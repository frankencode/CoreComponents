 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_SINGLETON_HPP
#define FTL_SINGLETON_HPP

#include "Instance.hpp"
#include "O.hpp"
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
	static SubClass *instance()
	{
		SpinLock &mutex = localStatic<SpinLock, SubClass>();
		Guard<SpinLock> guard(&mutex);
		O<SubClass> &instance_ = localStatic< O<SubClass>, CoreSingleton<SubClass> >();
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
	O<SubClass> instance_;
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

} // namespace ftl

#endif // FTL_SINGLETON_HPP
