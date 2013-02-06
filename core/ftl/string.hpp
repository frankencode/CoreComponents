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

class format;
class variant;

class string: public Ref<ByteArray>
{
public:
	typedef Ref<ByteArray> Super;

	string(): Super(ByteArray::empty()) {}
	explicit string(int size): Super(ByteArray::create(size)) {}
	string(int size, char zero): Super(ByteArray::create(size, zero)) {
		FTL_ASSERT(0 <= zero);
	}
	string(const Ref<ByteArray> &b): Super(b) {}
	string(ByteArray *b): Super(b) {}
	string(const string &b): Super(b.Super::get()) {}
	string(const format &b);
	string(const variant &b);
	string(Ref<StringList> parts);

	string(const char *data, int size = -1): Super(ByteArray::copy(data, size)) {}

	inline static string join(const StringList *parts, string sep = "") { return ByteArray::join(parts, sep); }

	inline string &operator=(const string &b) {
		Super::set(b.Super::get());
		return *this;
	}

	inline string &operator=(const char *data) {
		string b(data);
		Super::set(b.Super::get());
		return *this;
	}

	inline operator char *() const { return Super::get()->data(); }

private:
	friend class ByteArray;
};

inline Ref<StringList> operator+(const string &a, const string &b) {
	Ref<StringList> l = StringList::create();
	*l << a << b;
	return l;
}
inline Ref<StringList> operator+(Ref<StringList> &a, const string &b) { a << b; return a; }

inline bool operator< (const string &a, const string &b) { return str::cmp(a->data(), b->data()) <  0; }
inline bool operator==(const string &a, const string &b) { return str::cmp(a->data(), b->data()) == 0; }
inline bool operator> (const string &a, const string &b) { return str::cmp(a->data(), b->data()) >  0; }
inline bool operator!=(const string &a, const string &b) { return str::cmp(a->data(), b->data()) != 0; }
inline bool operator<=(const string &a, const string &b) { return str::cmp(a->data(), b->data()) <= 0; }
inline bool operator>=(const string &a, const string &b) { return str::cmp(a->data(), b->data()) >= 0; }

inline bool operator< (const char *a, const string &b) { return str::cmp(a, b->data()) <  0; }
inline bool operator==(const char *a, const string &b) { return str::cmp(a, b->data()) == 0; }
inline bool operator> (const char *a, const string &b) { return str::cmp(a, b->data()) >  0; }
inline bool operator!=(const char *a, const string &b) { return str::cmp(a, b->data()) != 0; }
inline bool operator<=(const char *a, const string &b) { return str::cmp(a, b->data()) <= 0; }
inline bool operator>=(const char *a, const string &b) { return str::cmp(a, b->data()) >= 0; }

inline bool operator< (char *a, const string &b) { return str::cmp(a, b->data()) <  0; }
inline bool operator==(char *a, const string &b) { return str::cmp(a, b->data()) == 0; }
inline bool operator> (char *a, const string &b) { return str::cmp(a, b->data()) >  0; }
inline bool operator!=(char *a, const string &b) { return str::cmp(a, b->data()) != 0; }
inline bool operator<=(char *a, const string &b) { return str::cmp(a, b->data()) <= 0; }
inline bool operator>=(char *a, const string &b) { return str::cmp(a, b->data()) >= 0; }

inline bool operator< (const string &a, const char *b) { return str::cmp(a->data(), b) <  0; }
inline bool operator==(const string &a, const char *b) { return str::cmp(a->data(), b) == 0; }
inline bool operator> (const string &a, const char *b) { return str::cmp(a->data(), b) >  0; }
inline bool operator!=(const string &a, const char *b) { return str::cmp(a->data(), b) != 0; }
inline bool operator<=(const string &a, const char *b) { return str::cmp(a->data(), b) <= 0; }
inline bool operator>=(const string &a, const char *b) { return str::cmp(a->data(), b) >= 0; }

inline bool operator< (const string &a, char *b) { return str::cmp(a->data(), b) <  0; }
inline bool operator==(const string &a, char *b) { return str::cmp(a->data(), b) == 0; }
inline bool operator> (const string &a, char *b) { return str::cmp(a->data(), b) >  0; }
inline bool operator!=(const string &a, char *b) { return str::cmp(a->data(), b) != 0; }
inline bool operator<=(const string &a, char *b) { return str::cmp(a->data(), b) <= 0; }
inline bool operator>=(const string &a, char *b) { return str::cmp(a->data(), b) >= 0; }

} // namespace ftl

#endif // FTL_STRING_HPP
