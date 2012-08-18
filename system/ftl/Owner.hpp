/*
 * Owner.hpp -- shared single-thread ownership 'Ref' policy
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_OWNER_HPP
#define FTL_OWNER_HPP

#include "defaults.hpp"
#ifdef FTL_REF_POLICY_RACE_DETECTION
#include <pthread.h>
#endif

namespace ftl
{

template<class T>
class Owner
{
public:
	Owner()
		: instance_(0)
	#ifdef FTL_REF_POLICY_RACE_DETECTION
		  , creator_(pthread_self())
	#endif
	{}

	inline void set(T *b)
	{
		#ifdef FTL_REF_POLICY_RACE_DETECTION
		if (creator_ != pthread_self())
			FTL_THROW(RefException, "Illegal concurrent assignment");
		#endif

		if (instance_ != b)
		{
			if (b)
				b->incRefCount();

			if (instance_)
				instance_->decRefCount();

			instance_ = b;
		}
	}

	inline T *get() const { return instance_; }

private:
	T *instance_;
	#ifdef FTL_REF_POLICY_RACE_DETECTION
	pthread_t creator_;
	#endif
};

template<class T>
inline Ref<T, Owner> newInstance() { return new T; }

template<class T, class A0>
inline Ref<T, Owner> newInstance(A0 a0) { return new T(a0); }

template<class T, class A0, class A1>
inline Ref<T, Owner> newInstance(A0 a0, A1 a1) { return new T(a0, a1); }

template<class T, class A0, class A1, class A2>
inline Ref<T, Owner> newInstance(A0 a0, A1 a1, A2 a2) { return new T(a0, a1, a2); }

template<class T, class A0, class A1, class A2, class A3>
inline Ref<T, Owner> newInstance(A0 a0, A1 a1, A2 a2, A3 a3) { return new T(a0, a1, a2, a3); }

} // namespace ftl

#endif // FTL_OWNER_HPP
