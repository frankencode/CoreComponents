/*
 * String.hpp -- UTF-8 strings
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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

class String: public Ref<ByteArray, Owner>
{
public:
	typedef Ref<ByteArray, Owner> Super;

	// initialize empty string
	String(): Super(ByteArray::empty()) {}

	// initialize string with defined size but undefined content
	explicit String(int size): Super(ByteArray::newInstance(size)) {}

	// initialize string with defined size and defined content
	String(int size, char zero): Super(ByteArray::newInstance(size, zero)) {
		FTL_ASSERT(0 <= zero);
	}

	// initialize string by given bytes
	String(const Ref<ByteArray, Owner> &bytes): Super(bytes) {}

	// initialize string from a shallow copy of another string
	String(const String &b): Super(b.Super::get()) {}

	// helper constructors
	String(const Format &b);
	String(const Variant &b);

	// initialize string by deep-copying a byte array
	String(const char *data, int size = -1): Super(ByteArray::newInstance(data, size)) {}

	// syntax sugar
	String(Ref<StringList, Owner> parts);
	Ref<StringList, Owner> operator+(const char *b) const { return *this + String(b); }
	Ref<StringList, Owner> operator+(const String &b) const;

	inline static String join(Ref<StringList> parts, String sep = "") { return ByteArray::join(parts, sep); }

	// assign a shallow copy of another string
	inline String &operator=(const String &b) {
		Super::set(b.Super::get());
		return *this;
	}

	// assign a deep copy of a byte array
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

	explicit String(const ByteArray *bytes): Super(bytes) {}
};

inline Ref<StringList, Owner> operator+(const char *a, const String &b) { return String(a) + b; }
inline Ref<StringList, Owner> operator+(Ref<StringList, Owner> a, const String &b) { a << b; return a; }

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
