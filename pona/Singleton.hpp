/*
 * Singleton.hpp -- process-wide singleton
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_SINGLETON_HPP
#define PONA_SINGLETON_HPP

#include "Instance.hpp"
#include "Ref.hpp"
#include "LocalStatic.hpp"

namespace pona
{

template<class SubClass>
class Singleton
{
public:
	static Ref<SubClass> instance()
	{
		SpinMutex& mutex = localStatic<SpinMutex, SubClass>();
		ScopeGuard<SpinMutex> guard(&mutex);
		Ref<SubClass, Owner>& instance_ = localStatic<Ref<SubClass, Owner>, SubClass>();
		if (!instance_)
			instance_ = new SubClass;
		return instance_;
	}
};

} // namespace pona

#endif // PONA_SINGLETON_HPP
