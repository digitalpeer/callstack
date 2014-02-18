/*
 * Based on Debug_GetCallstack.cpp from OpenLieroX
 *
 * Created by Albert Zeyer on 06.04.12.
 * code under LGPL
 */
#include "CMutex.h"
#include "CLockGuard.h"

#include <assert.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <unistd.h>

#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <sys/ucontext.h>

#include <iostream>
using namespace std;

static CMutex callstackMutex;
static pthread_t callingThread = 0;
static pthread_t targetThread = 0;
static void** threadCallstackBuffer = NULL;
static int threadCallstackBufferSize = 0;
static int threadCallstackCount = 0;

#define CALLSTACK_SIG SIGUSR2

void* GetPCFromUContext(void* secret)
{
   void* pnt = NULL;
#if defined(__x86_64__)
ucontext_t* uc = (ucontext_t*) secret;
   pnt = (void*) uc->uc_mcontext.gregs[REG_RIP] ;
#elif defined(__i386__)
   ucontext_t* uc = (ucontext_t*) secret;
   pnt = (void*) uc->uc_mcontext.gregs[REG_EIP] ;
#else
#error Unknown arch.
#endif
   return pnt;
}

__attribute__((noinline))
static void _callstack_signal_handler(int signr, siginfo_t *info, void *secret) 
{
   pthread_t myThread = (pthread_t)pthread_self();

   if (pthread_equal(myThread,targetThread) == 0) 
      return;

   cout << "_callstack_signal_handler, self: " << myThread << ", target: " << targetThread << ", caller: " << callingThread << endl;

   threadCallstackCount = backtrace(threadCallstackBuffer, threadCallstackBufferSize);

   // Search for the frame origin.
   for(int i = 1; i < threadCallstackCount; ++i) {
      if(threadCallstackBuffer[i] != NULL) continue;

      // Found it at stack[i]. Thus remove the first i.
      const int IgnoreTopFramesNum = i;
      threadCallstackCount -= IgnoreTopFramesNum;
      memmove(threadCallstackBuffer, threadCallstackBuffer + IgnoreTopFramesNum, threadCallstackCount * sizeof(void*));
      threadCallstackBuffer[0] = GetPCFromUContext(secret); // replace by real PC ptr
      break;
   }

   // continue calling thread
   pthread_kill((pthread_t)callingThread, CALLSTACK_SIG);
}

static void _setup_callstack_signal_handler() 
{
   struct sigaction sa;
   sigfillset(&sa.sa_mask);
   sa.sa_flags = SA_SIGINFO;
   sa.sa_sigaction = _callstack_signal_handler;
   sigaction(CALLSTACK_SIG, &sa, NULL);
}

__attribute__((noinline))
int GetCallstack(pthread_t threadId, void **buffer, int size) 
{
   if(threadId == 0 || threadId == (pthread_t)pthread_self()) {
      int count = backtrace(buffer, size);
      static const int IgnoreTopFramesNum = 1; // remove this `GetCallstack` frame
      if(count > IgnoreTopFramesNum) {
	 count -= IgnoreTopFramesNum;
	 memmove(buffer, buffer + IgnoreTopFramesNum, count * sizeof(void*));
      }
      return count;
   }

   CLockGuard lock(callstackMutex);
   callingThread = (pthread_t)pthread_self();
   targetThread = threadId;
   threadCallstackBuffer = buffer;
   threadCallstackBufferSize = size;

   _setup_callstack_signal_handler();

   // call _callstack_signal_handler in target thread
   if(pthread_kill((pthread_t)threadId, CALLSTACK_SIG) != 0)
   {
      assert(0);
      // something failed ...
      return 0;
   }

   {
      sigset_t mask;
      sigfillset(&mask);
      sigdelset(&mask, CALLSTACK_SIG);

      // wait for CALLSTACK_SIG on this thread
      sigsuspend(&mask);
   }

   threadCallstackBuffer = NULL;
   threadCallstackBufferSize = 0;
   return threadCallstackCount;
}


#include <iostream>
#include <execinfo.h>
#include <stdlib.h>

void print_backtrace(pthread_t tid)
{
   std::cout << "Backtrace for " << tid << std::endl;

   void* buffer[255];
   int size = GetCallstack(tid,buffer,sizeof(buffer));

   char** funcNames = backtrace_symbols( buffer, size );

   for (int i = 0; i < size; i++ )
      std::cout << funcNames[i] << std::endl;

   free( funcNames );

   std::cout << std::endl;
}
