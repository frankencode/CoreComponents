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

namespace ftl
{

/** \brief ownership pointer
  */
template<class T>
class hook
{
public:
	hook(): a_(0) {}
	~hook() { set(0); }

	hook(T *b): a_(0) { set(b); }
	hook(const hook &b): a_(0) { set(b.a_); }
	inline const hook &operator=(T *b) { set(b); return *this; }
	inline const hook &operator=(const hook &b) { set(b.a_); return *this; }
	inline operator T *() const { return a_; }

	template<class T2> hook(const hook<T2> &b): a_(0) { set(cast<T>(b.get())); }
	template<class T2> inline const hook &operator=(T2 *b) { set(cast<T>(b)); return *this; }
	template<class T2> inline const hook &operator=(const hook<T2> &b) { set(cast<T>(b.get())); return *this; }

	inline bool operator<(const hook &b) const { return a_ < b.a_; }

	inline T *operator->() const {
		FTL_ASSERT2(a_, "Null reference");
		return a_;
	}

	inline T *get() const { return a_; }

	inline void set(T *b) {
		if (a_ != b) {
			if (b) b->incRefCount();
			if (a_) a_->decRefCount();
			a_ = b;
		}
	}

	template<class T2>
	inline hook<T> &operator<<(T2 x) {
		FTL_ASSERT2(a_, "Null reference");
		*a_ << x;
		return *this;
	}

	template<class T2>
	inline hook<T> &operator>>(T2 &x) {
		FTL_ASSERT2(a_, "Null reference");
		*a_ >> x;
		return *this;
	}

private:
	T *a_;
};

} // namespace ftl

#endif // FTL_O_HPP
