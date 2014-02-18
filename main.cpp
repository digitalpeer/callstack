#include "callstack.h"
#include "CMutex.h"

#include <stdlib.h>
#include <pthread.h>
#include <execinfo.h>

#include <iostream>
using namespace std;

CMutex mutex;

void* thread1(void* arg)
{
   cout << "running thread 1 " << pthread_self() << endl;
   while (true)
   {
      for (unsigned int x = 0; x < 1000;x++)
      {
	 timespec timeout = {1,0};
	 if (!mutex.timedlock(timeout))
	    break;
	 if (x > 500)
	    break;
	 mutex.unlock();
      }
   }
   return 0;
}

void* thread2(void* arg)
{
   cout << "running thread 2 " << pthread_self() << endl;
   while (true)
   {
      for (unsigned int x = 0; x < 1000;x++)
      {
	 timespec timeout = {1,0};
	 if (!mutex.timedlock(timeout))
	    break;

	 if (x > 500)
	    break;
	 mutex.unlock();
      }
   }
   return 0;
}

int main()
{
   void* buffer[255];
   backtrace(buffer, sizeof(buffer));

   pthread_t tid1;
   pthread_create(&tid1,NULL,thread1,NULL);

   pthread_t tid2;
   pthread_create(&tid2,NULL,thread2,NULL);

   while (true)
   {
      sleep(1);
   }

   return 0;
}
