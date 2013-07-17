/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_SINGLETON_H
#define FKIT_SINGLETON_H

#include "Object.h"
#include "Ref.h"
#include "LocalStatic.h"
#include "ThreadLocalSingleton.h"

namespace fkit
{

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

} // namespace fkit

#endif // FKIT_SINGLETON_H
