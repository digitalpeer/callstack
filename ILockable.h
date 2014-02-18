#ifndef ILOCKABLE_H
#define ILOCKABLE_H

class ILockable
{
public:
   virtual void lock() = 0;
   virtual void unlock() = 0;

   virtual ~ILockable() {}
};

#endif

