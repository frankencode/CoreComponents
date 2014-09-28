/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_REF_H
#define FLUX_REF_H

#include "types.h"

namespace flux
{

/** \brief ownership pointer
  */
template<class T>
class Ref
{
public:
	Ref(): a_(0) {}
	~Ref() { set(0); }

	Ref(T *b): a_(0) { set(b); }
	Ref(const Ref &b): a_(0) { set(b.a_); }

	template<class T2> Ref(T2 *b): a_(0) { set(cast<T>(b)); }
	template<class T2> Ref(const Ref<T2> &b): a_(0) { set(cast<T>(b.get())); }

	inline const Ref &operator=(T *b) { set(b); return *this; }
	inline const Ref &operator=(const Ref &b) { set(b.a_); return *this; }
	inline operator T *() const { return a_; }

	template<class T2> inline const Ref &operator=(T2 *b) { set(cast<T>(b)); return *this; }
	template<class T2> inline const Ref &operator=(const Ref<T2> &b) { set(cast<T>(b.get())); return *this; }

	inline bool operator<(const Ref &b) const { return a_ < b.a_; }

	inline T *operator->() const {
		FLUX_ASSERT2(a_, "Null reference accessed");
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
	inline Ref<T> &operator<<(T2 x) {
		FLUX_ASSERT2(a_, "Null reference on shift left");
		*a_ << x;
		return *this;
	}

	template<class T2>
	inline Ref<T> &operator>>(T2 &x) {
		FLUX_ASSERT2(a_, "Null reference on shift right");
		*a_ >> x;
		return *this;
	}

private:
	T *a_;
};

template<class U, class T>
inline U *cast(const Ref<T>& p) { return cast<U>(p.get()); }

} // namespace flux

#endif // FLUX_REF_H