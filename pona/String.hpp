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
#include "List.hpp"

namespace pona
{

class String: public Ref<List<Char>, Owner>
{
public:
	typedef Char Element;
	typedef List<Char> Media;
	
	String();
	String(const Char& ch);
	String(const char* utf8, int numBytes = -1, int numChars = -1);
	String(Ref<Media, Owner> media);
	
	inline Ref<Media> media() const { return get(); }
	
	inline bool operator<(const String& b) const { return *get() < *b.get(); }
	inline bool operator==(const String& b) const { return *get() == *b.get(); }
	inline bool operator>(const String& b) const { return *get() > *b.get(); }
	inline bool operator!=(const String& b) const { return *get() != *b.get(); }
	inline bool operator<=(const String& b) const { return *get() <= *b.get(); }
	inline bool operator>=(const String& b) const { return *get() >= *b.get(); }
	
	inline String& operator<<(String b) { get()->append(b.get()); return *this; }
	inline String& operator<<(const char* b) { get()->append(String(b)); return *this; }
	inline String& operator<<(Char ch) { get()->append(ch); return *this; }
	
	char* strdup() const;
	CString utf8() const;
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

int toInt(String s, bool* ok = 0);
double toFloat(String s, bool* ok = 0);
int64_t toInt64(String s, bool* ok = 0);
uint64_t toUInt64(String s, bool* ok = 0);
float64_t toFloat64(String s, bool* ok = 0);

String toLower(String s);

} // namespace pona

#endif // PONA_STRING_HPP
