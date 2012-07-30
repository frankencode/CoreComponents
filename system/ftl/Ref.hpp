/*
 * Ref.hpp -- auto-casted object references
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_REF_HPP
#define FTL_REF_HPP

#include "defaults.hpp"
#include "types.hpp"
#include "Pointer.hpp"
#include "SetNull.hpp"
#include "Owner.hpp"
#include "OnDemand.hpp"

namespace ftl
{

/** \brief auto-casted object references
  *
  * Provides:
  *   - guarding against access after destruction
  *   - syntactic compatibility to C pointers
  *   - automatic dynamic typecasting on assignment
  *     (as in highlevel languages like Java or Python)
  *   - type-safety (as in Java, but unlike Python)
  *   - full compatibility to containers
  *
  * Notes on thread-safety:
  *   - reference counting on Instance::incRefCount(), Instance::decRefCount() is thread-safe
  *   - Owner and SetNull policies aren't thread-safe regarding concurrent assignment
  *     (race condition is detected if compiled with -DFTL_REF_POLICY_RACE_DETECTION)
  */
template<class T = Instance, template<class> class GetAndSetPolicy = FTL_DEFAULT_REF_POLICY>
class Ref: public GetAndSetPolicy<T>
{
public:
	// default initialization and cleanup

	Ref() {}
	~Ref() { this->set(reinterpret_cast<T*>(0)); }

	// non-casting initialization and copy operations

	Ref(const T* b) { this->set(const_cast<T*>(b)); }
	Ref(const Ref& b) { this->set(b.get()); }

	inline const Ref& operator=(T* b) { this->set(b); return *this; }
	inline const Ref& operator=(const Ref& b) { this->set(b.get()); return *this; }

	template<template<class> class GetAndSetPolicy2>
	explicit Ref(const Ref<T, GetAndSetPolicy2>& b) { this->set(b.get()); }

	template<template<class> class GetAndSetPolicy2>
	inline const Ref& operator=(const Ref<T, GetAndSetPolicy2>& b) { this->set(b.get()); return *this; }

	// auto-casting initialization and copy operations

	template<class T2, template<class> class GetAndSetPolicy2>
	explicit Ref(const Ref<T2, GetAndSetPolicy2>& b) { this->set(FTL_CAST_FROM_TO(T2, T, b.get())); }

	template<class T2, template<class> class GetAndSetPolicy2>
	inline const Ref& operator=(const Ref<T2, GetAndSetPolicy2>& b) { this->set(FTL_CAST_FROM_TO(T2, T, b.get())); return *this; }

	// non-casting conversions

	inline operator T*() const { return this->get(); }

	template<template<class> class GetAndSetPolicy2>
	inline operator Ref<T, GetAndSetPolicy2>() const { return Ref<T, GetAndSetPolicy2>(this->get()); }

	// auto-casting conversions

	template<class T2>
	inline operator Ref<T2, GetAndSetPolicy>() const { return Ref<T2, GetAndSetPolicy>(FTL_CAST_FROM_TO(T, T2, this->get())); }

	template<class T2, template<class> class GetAndSetPolicy2>
	inline operator Ref<T2, GetAndSetPolicy2>() const { return Ref<T2, GetAndSetPolicy2>(FTL_CAST_FROM_TO(T, T2, this->get())); }

	// ordering

	inline bool operator<(const Ref& b) const { return this->get() < b.get(); }

	// access

	inline T* operator->() const { return this->saveGet(); }

private:
	inline T* saveGet() const {
		T* instance = GetAndSetPolicy<T>::get();
		FTL_ASSERT2(instance, "Null reference");
		return instance;
	}
};

} // namespace ftl

#endif // FTL_REF_HPP
