/*
 * String.hpp -- string processing based on list processing
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_STRING_HPP
#define PONA_STRING_HPP

#include "atoms"
#include "CString.hpp"
#include "CharList.hpp"

namespace pona
{

class String: public Ref<CharList, Owner>
{
public:
	typedef Char Element;
	typedef CharList Media;
	
	String();
	String(const Char& ch);
	String(const char* utf8, int numBytes = -1, int numChars = -1);
	
	template<template<class> class SetAndGetPolicy>
	String(Ref<CharList::Super, SetAndGetPolicy> super) { set(super.cast<Media>()); }
	
	template<template<class> class SetAndGetPolicy>
	String(Ref<CharList, SetAndGetPolicy> media) { set(media); }
	
	// ensure string is never null
	inline virtual void set(Media* media) {
		if (!media)
			media = new Media;
		Owner<Media>::set(media);
	}
	
	inline bool operator<(const String& b) const { return *get() < *b.get(); }
	inline bool operator==(const String& b) const { return *get() == *b.get(); }
	inline bool operator>(const String& b) const { return *get() > *b.get(); }
	inline bool operator!=(const String& b) const { return *get() != *b.get(); }
	inline bool operator<=(const String& b) const { return *get() <= *b.get(); }
	inline bool operator>=(const String& b) const { return *get() >= *b.get(); }
	
	inline String& operator<<(String b) { get()->append(b.get()); return *this; }
	inline String& operator<<(const char* b) { get()->append(String(b)); return *this; }
	inline String& operator<<(Char ch) { get()->append(ch); return *this; }
};

inline bool operator<(String a, const char* b) { return a < String(b); }
inline bool operator==(String a, const char* b) { return a == String(b); }
inline bool operator>(String a, const char* b) { return a > String(b); }
inline bool operator!=(String a, const char* b) { return a != String(b); }
inline bool operator<=(String a, const char* b) { return a <= String(b); }
inline bool operator>=(String a, const char* b) { return a >= String(b); }
inline bool operator<(const char* b, String a) { return a < b; }
inline bool operator==(const char* b, String a) { return a == b; }
inline bool operator>(const char* b, String a) { return a > b; }
inline bool operator!=(const char* b, String a) { return a != b; }
inline bool operator<=(const char* b, String a) { return a <= b; }
inline bool operator>=(const char* b, String a) { return a >= b; }

inline String operator+(String a, String b) { String s; s << a << b; return s; }
inline String operator+(String a, const char* b) { return a + String(b); }
inline String operator+(const char* b, String a) { return a + b; }

String operator*(Char ch, int n);
inline String operator*(int n, Char ch) { return ch * n; }

typedef List<String> StringList;
Ref<StringList, Owner> operator/(String text, String sep);
String operator*(Ref<StringList> parts, String sep);
inline Ref<StringList, Owner> operator/(String text, const char* sep) { return text / String(sep); }
inline Ref<StringList, Owner> operator/(String text, Char sep) { return text / String(sep); }
inline Ref<StringList, Owner> operator/(String text, char sep) { return text / String(sep); }
inline String operator*(Ref<StringList> parts, const char* sep) { return parts * String(sep); }
inline String operator*(Ref<StringList> parts, Char sep) { return parts * String(sep); }
inline String operator*(Ref<StringList> parts, char sep) { return parts * String(sep); }

} // namespace pona

#endif // PONA_STRING_HPP
