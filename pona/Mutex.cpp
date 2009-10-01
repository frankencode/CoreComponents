#ifndef EBUSY
#include <errno.h>
#endif
#include "Mutex.hpp"

namespace pona
{

Mutex::Mutex()
{
	int ret = pthread_mutex_init(&mutex_, 0);
	if (ret != 0)
		PONA_THROW(SystemException, "pthread_mutex_init() failed");
}

Mutex::~Mutex()
{
	int ret = pthread_mutex_destroy(&mutex_);
	if (ret != 0)
		PONA_THROW(SystemException, "pthread_mutex_destroy() failed");
}

void Mutex::acquire()
{
	int ret = pthread_mutex_lock(&mutex_);
	if (ret != 0)
		PONA_THROW(SystemException, "pthread_mutex_lock() failed");
}

bool Mutex::tryAcquire()
{
	int ret = pthread_mutex_trylock(&mutex_);
	if ((ret != 0) && (ret != EBUSY))
		PONA_THROW(SystemException, "pthread_mutex_trylock() failed");
	return ret != EBUSY;
}

void Mutex::release()
{
	int ret = pthread_mutex_unlock(&mutex_);
	if (ret != 0)
		PONA_THROW(SystemException, "pthread_mutex_unlock() failed");
}

} // namespace pona
