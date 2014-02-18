#include "mutex.h"

#ifdef _cplusplus
export "C" {
#endif

#include <pthread.h>

#ifdef _cplusplus
}
#endif

namespace Common
{

namespace Threads
{

Mutex::Mutex() : mutex_(new pthread_mutex_t)
{
	pthread_mutex_init(reinterpret_cast<pthread_mutex_t*>(mutex_),0);
}

void
Mutex::Lock()
{
	pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(mutex_));
}

void
Mutex::Unlock()
{
	pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(mutex_));
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t*>(mutex_));
}

}

}
