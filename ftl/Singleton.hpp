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

namespace ftl
{

template<class SubClass>
class Singleton
{
public:
	static Ref<SubClass> instance()
	{
		CoreMutex& mutex = localStatic<CoreMutex, SubClass>();
		ScopeGuard<CoreMutex> guard(&mutex);
		Ref<SubClass, Owner>& instance_ = localStatic<Ref<SubClass, Owner>, SubClass>();
		if (!instance_)
			instance_ = new SubClass;
		return instance_;
	}
};

} // namespace ftl

#endif // FTL_SINGLETON_HPP
