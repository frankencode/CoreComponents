 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_STRING_HPP
#define FTL_STRING_HPP

#include "strings.hpp"
#include "ByteArray.hpp"
#include "Character.hpp"
#include "List.hpp"

namespace ftl
{

class Format;
class Variant;

class String: public O<ByteArray>
{
public:
	typedef O<ByteArray> Super;

	// initialize empty string
	String(): Super(ByteArray::empty()) {}

	// initialize string with defined size but undefined content
	explicit String(int size): Super(ByteArray::create(size)) {}

	// initialize string with defined size and defined content
	String(int size, char zero): Super(ByteArray::create(size, zero)) {
		FTL_ASSERT(0 <= zero);
	}

	// initialize string by given bytes
	String(const O<ByteArray> &bytes): Super(bytes) {}

	// initialize string from a shallow copy of another string
	String(const String &b): Super(b.Super::get()) {}

	// helper constructors
	String(const Format &b);
	String(const Variant &b);

	// initialize string by deep-copying a byte array
	String(const char *data, int size = -1): Super(ByteArray::create(data, size)) {}

	// syntax sugar
	String(O<StringList> parts);
	O<StringList> operator+(const char *b) const { return *this + String(b); }
	O<StringList> operator+(const String &b) const;

	inline static String join(const StringList *parts, String sep = "") { return ByteArray::join(parts, sep); }

	// assign a shallow copy of another string
	inline String &operator=(const String &b) {
		Super::set(b.Super::get());
		return *this;
	}

	// assign a deep copy of a C string
	inline String &operator=(const char *data) {
		String b(data);
		Super::set(b.Super::get());
		return *this;
	}

	// provide access to the shared bytes
	inline operator char*() const { return Super::get()->data(); }

	inline bool operator< (const String &b) const { return str::cmp((*this)->data(), b->data()) <  0; }
	inline bool operator==(const String &b) const { return str::cmp((*this)->data(), b->data()) == 0; }
	inline bool operator> (const String &b) const { return str::cmp((*this)->data(), b->data()) >  0; }
	inline bool operator!=(const String &b) const { return str::cmp((*this)->data(), b->data()) != 0; }
	inline bool operator<=(const String &b) const { return str::cmp((*this)->data(), b->data()) <= 0; }
	inline bool operator>=(const String &b) const { return str::cmp((*this)->data(), b->data()) >= 0; }

private:
	friend class ByteArray;

	explicit String(ByteArray *bytes): Super(bytes) {}
};

inline O<StringList> operator+(const char *a, const String &b) { return String(a) + b; }
inline O<StringList> operator+(O<ByteArray> a, const String &b) { return String(a) + b; }
inline O<StringList> operator+(O<StringList> a, const String &b) { *a << b; return a; }

inline bool operator< (const char *a, const String &b) { return str::cmp(a, b->data()) <  0; }
inline bool operator==(const char *a, const String &b) { return str::cmp(a, b->data()) == 0; }
inline bool operator> (const char *a, const String &b) { return str::cmp(a, b->data()) >  0; }
inline bool operator!=(const char *a, const String &b) { return str::cmp(a, b->data()) != 0; }
inline bool operator<=(const char *a, const String &b) { return str::cmp(a, b->data()) <= 0; }
inline bool operator>=(const char *a, const String &b) { return str::cmp(a, b->data()) >= 0; }

inline bool operator< (char *a, const String &b) { return str::cmp(a, b->data()) <  0; }
inline bool operator==(char *a, const String &b) { return str::cmp(a, b->data()) == 0; }
inline bool operator> (char *a, const String &b) { return str::cmp(a, b->data()) >  0; }
inline bool operator!=(char *a, const String &b) { return str::cmp(a, b->data()) != 0; }
inline bool operator<=(char *a, const String &b) { return str::cmp(a, b->data()) <= 0; }
inline bool operator>=(char *a, const String &b) { return str::cmp(a, b->data()) >= 0; }

inline bool operator< (const String &a, const char *b) { return str::cmp(a->data(), b) <  0; }
inline bool operator==(const String &a, const char *b) { return str::cmp(a->data(), b) == 0; }
inline bool operator> (const String &a, const char *b) { return str::cmp(a->data(), b) >  0; }
inline bool operator!=(const String &a, const char *b) { return str::cmp(a->data(), b) != 0; }
inline bool operator<=(const String &a, const char *b) { return str::cmp(a->data(), b) <= 0; }
inline bool operator>=(const String &a, const char *b) { return str::cmp(a->data(), b) >= 0; }

inline bool operator< (const String &a, char *b) { return str::cmp(a->data(), b) <  0; }
inline bool operator==(const String &a, char *b) { return str::cmp(a->data(), b) == 0; }
inline bool operator> (const String &a, char *b) { return str::cmp(a->data(), b) >  0; }
inline bool operator!=(const String &a, char *b) { return str::cmp(a->data(), b) != 0; }
inline bool operator<=(const String &a, char *b) { return str::cmp(a->data(), b) <= 0; }
inline bool operator>=(const String &a, char *b) { return str::cmp(a->data(), b) >= 0; }

} // namespace ftl

#endif // FTL_STRING_HPP
