/*
 * Singleton.hpp -- process-wide singleton
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_SINGLETON_HPP
#define FTL_SINGLETON_HPP

#include "Instance.hpp"
#include "Ref.hpp"
#include "LocalStatic.hpp"
#include "ThreadLocalSingleton.hpp"

namespace ftl
{

template<class SubClass>
class Singleton;

template<class SubClass>
class CoreSingleton
{
public:
	static Ref<SubClass> instance()
	{
		SpinLock& mutex = localStatic<SpinLock, SubClass>();
		Guard<SpinLock> guard(&mutex);
		Ref<SubClass, Owner>& instance_ = localStatic< Ref<SubClass, Owner>, CoreSingleton<SubClass> >();
		if (!instance_)
			instance_ = Singleton<SubClass>::create();
		return instance_;
	}
};

template<class SubClass>
class CoreSingletonWrapper: public Instance
{
public:
	CoreSingletonWrapper()
		: instance_(CoreSingleton<SubClass>::instance())
	{}
	Ref<SubClass, Owner> instance_;
};

template<class SubClass>
class Singleton: public ThreadLocalSingleton< CoreSingletonWrapper<SubClass> >
{
public:
	inline static Ref<SubClass> instance() {
		return ThreadLocalSingleton< CoreSingletonWrapper<SubClass> >::instance()->instance_;
	}
private:
	friend class CoreSingleton<SubClass>;
	inline static SubClass* create() { return new SubClass; }
};

} // namespace ftl

#endif // FTL_SINGLETON_HPP
