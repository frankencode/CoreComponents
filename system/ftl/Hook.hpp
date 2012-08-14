/*
 * Hook.hpp -- shared object ownership hooks
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_HOOK_HPP
#define FTL_HOOK_HPP

#include "defaults.hpp"
#include "types.hpp"

namespace ftl
{

template<class T>
class Hook
{
public:
	// default initialization and cleanup

	Hook(): instance_(0) {}
	~Hook() { set(reinterpret_cast<T*>(0)); }

	// non-casting initialization and copy operations

	Hook(const T* b) { set(const_cast<T*>(b)); }
	Hook(const Hook& b) { set(b.get()); }
	Hook(const Ref<T>& b) { set(b.get()); }

	inline const Hook& operator=(T* b) { set(b); return *this; }
	inline const Hook& operator=(const Hook& b) { set(b.get()); return *this; }
	inline const Hook& operator=(const Ref<T>& b) { set(b.get()); return *this; }

	// auto-casting initialization and copy operations

	template<class T2>
	explicit Hook(const Ref<T2>& b) { set(FTL_CAST_FROM_TO(T2, T, b.get())); }

	template<class T2>
	inline const Hook& operator=(const Ref<T2>& b) { set(FTL_CAST_FROM_TO(T2, T, b.get())); return *this; }

	// non-casting conversions

	inline operator T*() const { return get(); }
	inline operator Ref<T>() const { return Ref<T>(get()); }

	// auto-casting conversions

	template<class T2>
	inline operator Hook<T2>() const { return Hook<T2>(FTL_CAST_FROM_TO(T, T2, get())); }

	template<class T2>
	inline operator Ref<T2>() const { return Ref<T2>(FTL_CAST_FROM_TO(T, T2, get())); }

	// ordering

	inline bool operator<(const Ref& b) const { return get() < b.get(); }

	// access

	inline T* operator->() const { return saveGet(); }

protected:
	inline void set(T* b)
	{
		if (instance_ != b)
		{
			if (b)
				b->incRefCount();

			if (instance_)
				instance_->decRefCount();

			instance_ = b;
		}
	}

	inline T* get() const { return instance_; }

	inline T* saveGet() const {
		FTL_ASSERT2(instance_, "Null reference");
		return instance_;
	}

	T* instance_;
};

} // namespace ftl

#endif // FTL_HOOK_HPP
