/*
 * ThreadLocalSingleton.hpp -- thread-local singleton
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_THREADLOCALSINGLETON_HPP
#define FTL_THREADLOCALSINGLETON_HPP

#include "Instance.hpp"
#include "Ref.hpp"
#include "ThreadLocalOwner.hpp"
#include "LocalStatic.hpp"

namespace ftl
{

template<class SubClass>
class ThreadLocalSingleton
{
public:
	static Ref<SubClass> instance()
	{
		Ref<SubClass, ThreadLocalOwner>& instance_ = localStatic<Ref<SubClass, ThreadLocalOwner>, SubClass>();
		if (!instance_)
			instance_ = new SubClass;
		return instance_;
	}
};

} // namespace ftl

#endif // FTL_THREADLOCALSINGLETON_HPP
