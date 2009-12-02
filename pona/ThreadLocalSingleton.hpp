/*
 * ThreadLocalSingleton.hpp -- thread-local singleton
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_THREADLOCALSINGLETON_HPP
#define PONA_THREADLOCALSINGLETON_HPP

#include "Instance.hpp"
#include "Ref.hpp"
#include "ThreadLocalOwner.hpp"
#include "LocalStatic.hpp"

namespace pona
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

} // namespace pona

#endif // PONA_THREADLOCALSINGLETON_HPP
