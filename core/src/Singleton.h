/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_SINGLETON_H
#define FLUX_SINGLETON_H

#include <flux/Object>
#include <flux/Ref>
#include <flux/LocalStatic>
#include <flux/ThreadLocalSingleton>

namespace flux {

template<class SubClass>
class Singleton;

template<class SubClass>
class CoreSingleton
{
public:
	static SubClass *instance()
	{
		SpinLock &mutex = localStatic<SpinLock, SubClass>();
		Guard<SpinLock> guard(&mutex);
		Ref<SubClass> &instance_ = localStatic< Ref<SubClass>, CoreSingleton<SubClass> >();
		if (!instance_)
			instance_ = Singleton<SubClass>::create();
		return instance_;
	}
};

template<class SubClass>
class CoreSingletonWrapper: public Object
{
public:
	CoreSingletonWrapper()
		: instance_(CoreSingleton<SubClass>::instance())
	{}
	Ref<SubClass> instance_;
};

template<class SubClass>
class Singleton: public ThreadLocalSingleton< CoreSingletonWrapper<SubClass> >
{
public:
	inline static SubClass *instance() {
		return ThreadLocalSingleton< CoreSingletonWrapper<SubClass> >::instance()->instance_;
	}
private:
	friend class CoreSingleton<SubClass>;
	inline static SubClass *create() { return new SubClass; }
};

} // namespace flux

#endif // FLUX_SINGLETON_H
