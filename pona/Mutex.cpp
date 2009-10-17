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
		PONA_PTHREAD_EXCEPTION("pthread_mutex_init", ret);
}

Mutex::~Mutex()
{
	int ret = pthread_mutex_destroy(&mutex_);
	if (ret != 0)
		PONA_PTHREAD_EXCEPTION("pthread_mutex_destroy", ret);
}

void Mutex::acquire()
{
	int ret = -1;
	while (true) {
		ret = pthread_mutex_lock(&mutex_);
		if (ret != EINTR) break;
	}
	if (ret != 0)
		PONA_PTHREAD_EXCEPTION("pthread_mutex_lock", ret);
}

bool Mutex::tryAcquire()
{
	int ret = -1;
	while (true) {
		ret = pthread_mutex_trylock(&mutex_);
		if (ret != EINTR) break;
	}
	if ((ret != 0) && (ret != EBUSY))
		PONA_PTHREAD_EXCEPTION("pthread_mutex_trylock", ret);
	return ret != EBUSY;
}

void Mutex::release()
{
	int ret = pthread_mutex_unlock(&mutex_);
	if (ret != 0)
		PONA_PTHREAD_EXCEPTION("pthread_mutex_unlock", ret);
}

} // namespace pona
