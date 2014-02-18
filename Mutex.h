#include "ILockable.h"

/**
 @class Mutex
 Simple mutex wrapper class around the pthread mutex implimentations.
*/
class Mutex : public ILockable
{
public:
	/**
	 Create the mutex.
	*/
	Mutex();

	/**
	 Lock the mutex.
	*/
	virtual void lock();

	/**
	 Unlock the mutex.
	*/
	virtual void unlock();

	/**
	 Delete the mutex.
	*/
	virtual ~Mutex();
private:

	/**
	 Internal pointer to the real mutex object we are using.
	*/
	void* mutex_;
};
