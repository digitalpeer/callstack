#ifndef CMUTEX_H
#define CMUTEX_H

#include "ILockable.h"
#include <pthread.h>
#include <time.h>

class CMutex : public ILockable
{
public:

   CMutex();

   virtual void lock();
   virtual bool trylock();
   virtual bool timedlock(const timespec& relative);
   
   virtual void unlock();

   virtual ~CMutex();

private:

   pthread_mutex_t* m_mutex;
   pthread_t m_owner;
};

#endif
