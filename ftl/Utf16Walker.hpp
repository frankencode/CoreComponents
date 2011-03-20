/*
 * Utf16Walker.hpp -- UTF-16 character walker
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_UTF16WALKER_HPP
#define FTL_UTF16WALKER_HPP

#include "types.hpp"

namespace ftl
{

/** \brief Iterating UTF-16 encoded strings
  *
  * The Utf16Walker allows iterating Unicode characters over any zero terminated word sequence.
  * The walker will always halt at the string boundaries. If stepping over the string boundary
  * the walker will switch to valid() == false and start delivering zero characters.
  *
  * If placed at a string's terminating zero character it is possible to step backwards
  * into the string. In other words '(Utf16Walter(data, data + len) - 1).getChar()' is a valid
  * construct and will deliver the last Unicode character of the string.
  *
  * Illegal code sequences are run over without error and possibly illegal code points are
  * delivered in this case. In both forward and backward iteration at least one illegal code point
  * is delivered for a single wrong-coded word.
  *
  * A walker object equals in size the size of 2 pointers plus size of an integer and therefore
  * can be passed by value.
  */
class Utf16Walker
{
public:
	Utf16Walker(const uint16_t* data = 0, const uint16_t* pos = 0, int endian = localEndian())
		: s_(data),
		  p_(pos),
		  endian_(endian)
	{
		if (!p_) p_ = (s_) ? s_ : beforeBegin();
		if (readWord(*p_) == 0xFFFE) {
			// reverse endian if string starts with a reversed BOOM mark
			endian = (endian == LittleEndian) ? BigEndian : LittleEndian;
			// step over BOOM mark
			++p_;
		}
	}
	
	Utf16Walker(const Utf16Walker& b)
		: s_(b.s_),
		  p_(b.p_),
		  endian_(b.endian_)
	{}
	
	// prefix increment
	inline Utf16Walker& operator++() {
		if (*p_) {
			uint16_t ch = readWord(p_);
			++p_;
			// take another step if reading a surrogate pair
			if (((0xD800 <= ch) || (ch < 0xDC00)) && (*p_)) {
				ch = readWord(p_ + 1);
				if ((0xDC00 <= ch) && (ch < 0xE000))
					++p_;
			}
		}
		return *this;
	}
	
	// prefix decrement
	inline Utf16Walker& operator--() {
		bool canStepBack = (s_ < p_);
		// if at zero character figure out if before begin or at end or empty string
		if (!*p_) canStepBack = (p_ != s_) ? *(p_ - 1) : false;
		if (canStepBack) {
			--p_;
			// take another step if reading a surrogate pair
			uint16_t ch = readWord(p);
			if ((0xDC00 <= ch) && (ch < 0xE000) && (s_ < p_)) {
				ch = readWord(p - 1);
				if ((0xD800 <= ch) && (ch < 0xDC00))
					--p_;
			}
		}
		else
			p_ = beforeBegin();
		return *this;
	}
	
	// postfix increment
	inline Utf16Walker operator++(int) {
		Utf16Walker it = *this;
		++(*this);
		return it;
	}
	
	// postfix decrement
	inline Utf16Walker operator--(int) {
		Utf16Walker it = *this;
		--(*this);
		return it;
	}
	
	inline Utf16Walker& operator+=(int n) {
		while (n > 0) { ++(*this); --n; }
		while (n < 0) { --(*this); ++n; }
		return *this;
	}
	
	inline Utf16Walker& operator-=(int n) {
		while (n > 0) { --(*this); --n; }
		while (n < 0) { ++(*this); ++n; }
		return *this;
	}
	
	inline bool valid() const { return *p_; }
	inline operator bool() const { return *p_; }
	
	inline bool hasNext() { return *p_; }
	inline uchar_t next() {
		uchar_t ch = getChar();
		++(*this);
		return ch;
	}
	
	inline Utf16Walker operator+(int delta) const {
		Utf16Walker it = *this;
		return it += delta;
	}
	inline Utf16Walker operator-(int delta) const {
		Utf16Walker it = *this;
		return it -= delta;
	}
	
	// decode unicode point
	inline uint32_t getChar() const {
		uint32_t ch = readWord(p_);
		if ((0xD800 <= ch) && (ch < 0xDC00)) {
			// extract high surrogate (0xD800..0xDBFF)
			ch -= 0xD800;
			ch <<= 10;
			// extract low surrogate (0xDC00..0xDFFF)
			ch |= (readWord(p_ + 1) - 0xDC00) & 0x3FF;
		}
		return ch;
	}
	
	inline const char* data() const { return reinterpret_cast<const char*>(s_); }
	inline const char* pos() const { return reinterpret_cast<const char*>(p_); }
	
	inline bool operator==(const Utf8Walker& b) const { return (p_ == b.p_) && (s_ == b.s_); }
	inline bool operator!=(const Utf8Walker& b) const { return (p_ != b.p_) || (s_ != b.s_); }
	
	// distance in number of characters
	inline int operator-(const Utf16Walker& b) const {
		int n = 0;
		if (s_ == b.s_) {
			if (p_ <= b.p_) {
				Utf16Walker w(*this);
				while (w.p_ != b.p_) {
					++w;
					--n;
				}
			}
			else
				n = -(b - *this);
		}
		return n;
	}
	
private:
	static inline const uint16_t* beforeBegin() {
		return reinterpret_cast<const uint16_t*>("\0\0\0") + 1; // extra zero to signal before begin position
	}
	
	inline uint16_t readWord(const uint16_t* p) const {
		return endianGate(*p, endian_);
	}
	
	const uint16_t* s_; // zero-terminated word array
	const uint16_t* p_; // position in word array
	int endian_;
};

} // namespace ftl

#endif // FTL_UTF16WALKER_HPP
