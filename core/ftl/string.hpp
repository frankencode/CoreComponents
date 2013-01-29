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

class string: public hook<ByteArray>
{
public:
	typedef hook<ByteArray> Super;

	// initialize empty string
	string(): Super(ByteArray::empty()) {}

	// initialize string with defined size but undefined content
	explicit string(int size): Super(ByteArray::create(size)) {}

	// initialize string with defined size and defined content
	string(int size, char zero): Super(ByteArray::create(size, zero)) {
		FTL_ASSERT(0 <= zero);
	}

	// initialize string by given bytes
	string(const hook<ByteArray> &bytes): Super(bytes) {}

	// initialize string from a shallow copy of another string
	string(const string &b): Super(b.Super::get()) {}

	// helper constructors
	string(const format &b);
	string(const variant &b);
	string(hook<StringList> parts);

	// initialize string by deep-copying a byte array
	string(const char *data, int size = -1): Super(ByteArray::create(data, size)) {}

	inline static string join(const StringList *parts, string sep = "") { return ByteArray::join(parts, sep); }

	// assign a shallow copy of another string
	inline string &operator=(const string &b) {
		Super::set(b.Super::get());
		return *this;
	}

	// assign a deep copy of a C string
	inline string &operator=(const char *data) {
		string b(data);
		Super::set(b.Super::get());
		return *this;
	}

	// provide access to the shared bytes
	inline operator char *() const { return Super::get()->data(); }

private:
	friend class ByteArray;

	explicit string(ByteArray *bytes): Super(bytes) {}
};

inline hook<StringList> operator+(const string &a, const string &b) {
	hook<StringList> l = StringList::create();
	*l << a << b;
	return l;
}
inline hook<StringList> operator+(hook<StringList> &a, const string &b) { a << b; return a; }

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
