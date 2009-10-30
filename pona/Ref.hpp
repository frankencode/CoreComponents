/*
 * Ref.hpp -- auto-casted object reference
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_REF_HPP
#define PONA_REF_HPP

#include "defaults.hpp"
#include "Instance.hpp"
#include "Pointer.hpp"
#include "SetNull.hpp"
#include "Owner.hpp"

namespace pona
{

PONA_EXCEPTION(RefException, Exception);

/** \brief auto-casted object reference
  *
  * provides:
  *   - guarding against access after destruction
  *   - syntactic compatibility to C pointers
  *   - automatic dynamic typecasting on assignment
  *     (as in highlevel languages like Java or Python)
  *   - full compatiblity to containers
  */
template<class T = Instance, template<class> class GetAndSetPolicy = PONA_DEFAULT_REF_POLICY>
class Ref: public GetAndSetPolicy<T>
{
public:
	// default initialization and cleanup
	
	Ref() {}
	~Ref() { set(reinterpret_cast<T*>(0)); }
	
	// non-casting initialization and copy operations
	
	Ref(T* b) { set(b); }
	Ref(const Ref& b) { set(b.get()); }
	template<template<class> class GetAndSetPolicy2>
	explicit Ref(const Ref<T, GetAndSetPolicy2>& b) { set(b.get()); }
	
	inline const Ref& operator=(T* b) { set(b); return *this; }
	inline const Ref& operator=(const Ref& b) { set(b.get()); return *this; }
	
	// auto-casting initialization and copy operations
	
	template<class T2, template<class> class GetAndSetPolicy2>
	explicit Ref(const Ref<T2, GetAndSetPolicy2>& b) { set(dynamic_cast<T*>(b.get())); }
	
	template<template<class> class GetAndSetPolicy2>
	inline const Ref& operator=(const Ref<T, GetAndSetPolicy2>& b) { set(b.get()); return *this; }
	
	template<class T2, template<class> class GetAndSetPolicy2>
	inline const Ref& operator=(const Ref<T2, GetAndSetPolicy2>& b) { set(dynamic_cast<T*>(b.get())); return *this; }
	
	// automatic casts
	
	inline operator T*() const { return this->get(); }
	
	template<template<class> class GetAndSetPolicy2>
	inline operator Ref<T, GetAndSetPolicy2>() const { return Ref<T, GetAndSetPolicy2>(this->get()); }
	
	template<class T2>
	inline operator Ref<T2, GetAndSetPolicy>() const { return Ref<T2, GetAndSetPolicy>(dynamic_cast<T2*>(this->get())); }
	
	template<class T2, template<class> class GetAndSetPolicy2>
	inline operator Ref<T2, GetAndSetPolicy2>() const { return Ref<T2, GetAndSetPolicy2>(dynamic_cast<T2*>(this->get())); }
	
	// ordering
	
	inline bool operator<(const Ref& b) const { return *(this->saveGet()) < *(b.saveGet()); }
	
	// access
	
	inline T* operator->() const { return this->saveGet(); }
	
private:
	inline T* saveGet() const {
		T* instance = this->get();
		#ifndef NDEBUG
		if (!instance)
			PONA_THROW(RefException, "Null reference");
		#endif
		return instance;
	}
};

} // namespace pona

#endif // PONA_REF_HPP
