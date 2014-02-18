#include "CMutex.h"
#include "callstack.h"
#include <iostream>
#include <errno.h>
#include <cassert>

using namespace std;

CMutex::CMutex() 
   : m_mutex(new pthread_mutex_t),
     m_owner(0)
{
   pthread_mutex_init(m_mutex,0);
}

void CMutex::lock()
{
   int ret = pthread_mutex_lock(m_mutex);
   if (ret == 0)
   {
      m_owner = pthread_self();
      return;
   }

   assert(0);
}

void CMutex::unlock()
{
   m_owner = 0;
   pthread_mutex_unlock(m_mutex);
}

bool CMutex::trylock()
{
   int ret = pthread_mutex_trylock(m_mutex);

   if (ret == 0)
   {
      m_owner = pthread_self();
      return true;
   }

   return false;
}

static timespec diff(timespec start, timespec end)
{
   timespec temp;
   if ((end.tv_nsec-start.tv_nsec)<0) {
      temp.tv_sec = end.tv_sec-start.tv_sec-1;
      temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
   } else {
      temp.tv_sec = end.tv_sec-start.tv_sec;
      temp.tv_nsec = end.tv_nsec-start.tv_nsec;
   }
   return temp;
}

bool operator<(const timespec& lhs,const timespec& rhs)
{
   if (lhs.tv_sec < rhs.tv_sec)
      return true;
   else if (lhs.tv_sec > rhs.tv_sec) 
      return false;

   if (lhs.tv_nsec < rhs.tv_nsec)
      return true;

   return false;
}

bool CMutex::timedlock(const timespec& relative)
{
   struct timespec start;
   clock_gettime(CLOCK_REALTIME,&start);

   struct timespec now;

   do
   {
      if (trylock())
	 return true;

      clock_gettime(CLOCK_REALTIME,&now);
   } 
   while (diff(start,now) < relative);

   cout << "Failed to lock in " << pthread_self() << " locked by " << m_owner << endl;
   print_backtrace(m_owner);
   
   return false;
}

CMutex::~CMutex()
{
   pthread_mutex_destroy(m_mutex);
}
