 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_O_HPP
#define FTL_O_HPP

#include "types.hpp"
#include "Exception.hpp"

namespace ftl
{

/** \internal
  * \brief Ownership Pointer
  */
template<class T>
class O
{
public:
	O(T *b = 0): a_(0) { set(b); }
	O(const T &b): a_(0) { set(b.a_); }
	~O() { set(reinterpret_cast<T *>(0)); }

	inline const O &operator=(T *b) { set(b); return *this; }
	inline const O &operator=(const O &b) { set(b.a_); return *this; }

	inline operator T *() const { return a_; }

	inline T *operator->() const {
		FTL_ASSERT2(a_, "Null reference");
		return a_;
	}

	template<class I>
	inline T &operator<<(const I &item) {
		FTL_ASSERT2(a_, "Null reference");
		return *a_ << item;
	}

private:
	inline void set(T *b) {
		if (a_ != b) {
			if (b) b->incRefCount();
			if (a_) a_->decRefCount();
			a_ = b;
		}
	}

	T *a_;
};

} // namespace ftl

#endif // FTL_O_HPP
