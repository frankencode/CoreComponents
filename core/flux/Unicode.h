/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_UNICODE_H
#define FLUX_UNICODE_H

#include "ByteArray.h"
#include "Utf8Walker.h"

namespace flux
{

class Unicode: public Object
{
public:
	inline static Ref<Unicode> open(ByteArray *data)
	{
		return new Unicode(data);
	}

	inline bool has(int i) const {
		walkTo(i);
		return walker_.valid();
	}

	inline uchar_t at(int i) const {
		walkTo(i);
		return walker_.getChar();
	}

	inline int count() const {
		if (n_ == -1) {
			if (!walker_.valid()) {
				walker_ = Utf8Walker(walker_.data());
				i_ = 0;
			}
			while (walker_.valid()) { ++walker_; ++i_; }
			n_ = i_;
			if (i_ > 0) { --walker_; --i_; }
		}
		return n_;
	}

	Ref<ByteArray> copy(int i0, int i1) const;
	inline Ref<ByteArray> head(int n) const { return copy(0, n); }
	inline Ref<ByteArray> tail(int n) const { return copy(count() - n, n); }

	inline const char *byte(int i) const {
		walkTo(i);
		return walker_.pos();
	}

	inline int index(const char *pos) const {
		if (!walker_.valid()) {
			walker_ = Utf8Walker(walker_.data());
			i_ = 0;
		}
		while (walker_.pos() < pos) { ++walker_; ++i_; }
		while (walker_.pos() > pos) { --walker_; --i_; }
		return i_;
	}

private:
	Unicode(ByteArray *data)
		: data_(data),
		  walker_(data->chars()),
		  i_(0), n_(-1)
	{}

	inline void walkTo(int i) const {
		if (!walker_.valid()) {
			walker_ = Utf8Walker(walker_.data());
			i_ = 0;
		}
		while (i_ < i) { ++walker_; ++i_; }
		while (i_ > i) { --walker_; --i_; }
	}

	Ref<ByteArray> data_;
	mutable Utf8Walker walker_;
	mutable int i_, n_;
};

} // namespace flux

#endif // FLUX_UNICODE_H
