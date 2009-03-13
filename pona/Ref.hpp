/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_REF_HPP
#define PONA_REF_HPP

#include "options.hpp"
#include "Instance.hpp"
#include "Pointer.hpp"
#include "Owner.hpp"
#include "SetNull.hpp"
#include "ThreadLocalOwner.hpp"

namespace pona
{

/** \brief Auto-casted object reference
  *
  * see http://en.wikipedia.org/wiki/Reference_(computer_science)
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
	inline operator Ref<T2, GetAndSetPolicy2>() const { return Ref<T2, GetAndSetPolicy2>(dynamic_cast<T2*>(this->get()));  }
	
	// ordering
	inline bool operator<(const Ref& b) const {
		T* ai = this->get();
		T* bi = b.get();
		if ((!ai) || (!bi))
			*(char*)0 = 0; // access to undefined object requested
		return *ai < *bi;
	}
	
	// access
	inline T* operator->() const {
		T* instance = this->get();
		if (!instance) *(char*)0 = 0;
		return instance;
	}
};

} // namespace pona

#endif // PONA_REF_HPP
