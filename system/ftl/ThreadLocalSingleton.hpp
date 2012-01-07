/*
 * ThreadLocalSingleton.hpp -- thread-local singleton
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
		Ref<SubClass, ThreadLocalOwner>& instance_ = localStatic< Ref<SubClass, ThreadLocalOwner>, ThreadLocalSingleton<SubClass> >();
		if (!instance_)
			instance_ = new SubClass;
		return instance_;
	}
};

} // namespace ftl

#endif // FTL_THREADLOCALSINGLETON_HPP
