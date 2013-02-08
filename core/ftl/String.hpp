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

class String: public Ref<ByteArray>
{
public:
	typedef Ref<ByteArray> Super;

	String(): Super(ByteArray::empty()) {}
	explicit String(int size): Super(ByteArray::create(size)) {}
	String(int size, char zero): Super(ByteArray::create(size, zero)) {
		FTL_ASSERT(0 <= zero);
	}
	String(const Ref<ByteArray> &b): Super(b) {}
	String(ByteArray *b): Super(b) {}
	String(const String &b): Super(b.Super::get()) {}
	String(const Format &b);
	String(const Variant &b);
	String(Ref<StringList> parts);

	String(const char *data, int size = -1): Super(ByteArray::copy(data, size)) {}

	inline static String join(const StringList *parts, String sep = "") { return ByteArray::join(parts, sep); }

	inline String &operator=(const String &b) {
		Super::set(b.Super::get());
		return *this;
	}

	inline String &operator=(const char *data) {
		String b(data);
		Super::set(b.Super::get());
		return *this;
	}

	inline operator char *() const { return Super::get()->data(); }

private:
	friend class ByteArray;
};

inline Ref<StringList> operator+(const String &a, const String &b) {
	Ref<StringList> l = StringList::create();
	*l << a << b;
	return l;
}
inline Ref<StringList> operator+(Ref<StringList> &a, const String &b) { a << b; return a; }

inline bool operator< (const String &a, const String &b) { return str::cmp(a->data(), b->data()) <  0; }
inline bool operator==(const String &a, const String &b) { return str::cmp(a->data(), b->data()) == 0; }
inline bool operator> (const String &a, const String &b) { return str::cmp(a->data(), b->data()) >  0; }
inline bool operator!=(const String &a, const String &b) { return str::cmp(a->data(), b->data()) != 0; }
inline bool operator<=(const String &a, const String &b) { return str::cmp(a->data(), b->data()) <= 0; }
inline bool operator>=(const String &a, const String &b) { return str::cmp(a->data(), b->data()) >= 0; }

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
