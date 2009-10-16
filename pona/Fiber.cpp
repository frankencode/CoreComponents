/*
 * Fiber.cpp -- fibers, also know as coroutines
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <pthread.h>
#include "Exception.hpp"
#include "Fiber.hpp"

namespace pona
{

pthread_key_t fiberEntry_;

void Fiber::bootstrap()
{
	Fiber* fiber = reinterpret_cast<Fiber*>(::pthread_getspecific(fiberEntry_));
	fiber->run();
	if (!fiber->uc_link)
		fiber->yield();
}

int FiberInitializer::count_ = 0;

FiberInitializer::FiberInitializer()
{
	if (count_ == 0) {
		++count_;
		int ret = pthread_key_create(&fiberEntry_, 0);
		if (ret != 0)
			PONA_PTHREAD_EXCEPTION("pthread_key_create", ret);
	}
}

FiberInitializer::~FiberInitializer()
{
	pthread_key_delete(fiberEntry_);
}

Fiber::Fiber(int stackSize, int guardSize, Fiber* next)
{
	int ret = pthread_setspecific(fiberEntry_, this);
	if (ret != 0)
		PONA_PTHREAD_EXCEPTION("pthread_setspecific", ret);
	Context::get(this);
	Context::make(this, &bootstrap, stackSize, guardSize, next);
}

void Fiber::resume()
{
	Context::swap(&saved_, this);
}

void Fiber::transferTo(Fiber* target)
{
	Context::swap(this, target);
}

void Fiber::yield()
{
	Context::swap(this, &saved_);
}

} // namespace pona
