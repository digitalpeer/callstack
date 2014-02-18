#ifndef CLOCKGUARD_H
#define CLOCKGUARD_H

#include "ILockable.h"

class CLockGuard
{
public:

   CLockGuard(ILockable& lockable)
      : m_lock(lockable)
   {
      m_lock.lock();
   }

   ~CLockGuard()
   {
      m_lock.unlock();
   }

protected:
   ILockable& m_lock;
};

#endif
