/*
 * Character.hpp -- access UTF-8 encoded characters by index
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_CHARACTER_HPP
#define FTL_CHARACTER_HPP

#include "atoms"
#include "Array.hpp"

namespace ftl
{

class Character: public Instance
{
public:
	Character(Ref<ByteArray> bytes)
		: walker_(bytes->constData()),
		  i_(0), n_(-1)
	{}
	
	inline bool has(int i) const {
		walkTo(i);
		return walker_.valid();
	}
	
	inline uchar_t get(int i) const {
		walkTo(i);
		return walker_.getChar();
	}
	
	inline int length() const {
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
	
	Ref<ByteArray, Owner> copy(int i0, int i1) const {
		FTL_ASSERT(i0 <= i1);
		walkTo(i0);
		const char* p0 = walker_.pos();
		while (i_ < i1) { ++walker_; ++i_; }
		return new ByteArray(p0, walker_.pos() - p0);
	}
	
	inline uchar_t operator[](int i) const { return get(i); }
	
	inline const char* byte(int i) const {
		walkTo(i);
		return walker_.pos();
	}
	
	inline int index(const char* pos) const {
		if (!walker_.valid()) {
			walker_ = Utf8Walker(walker_.data());
			i_ = 0;
		}
		while (walker_.pos() < pos) { ++walker_; ++i_; }
		while (walker_.pos() > pos) { --walker_; --i_; }
		return i_;
	}
	
private:
	inline void walkTo(int i) const {
		if (!walker_.valid()) {
			walker_ = Utf8Walker(walker_.data());
			i_ = 0;
		}
		while (i_ < i) { ++walker_; ++i_; }
		while (i_ > i) { --walker_; --i_; }
	}
	
	mutable Utf8Walker walker_;
	mutable int i_, n_;
};

} // namespace ftl

#endif // FTL_CHARACTER_HPP
