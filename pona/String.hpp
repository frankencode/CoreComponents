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

class PONA_API String: public Ref<List<Char>, Owner>
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
	
	CString utf8() const;
	char* strdup() const;
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

PONA_API String operator*(Char ch, int n);
inline String operator*(int n, Char ch) { return ch * n; }

typedef List<String> StringList;
PONA_API Ref<StringList, Owner> operator/(String text, String sep);
PONA_API String operator*(Ref<StringList> parts, String sep);
inline Ref<StringList, Owner> operator/(String text, const char* sep) { return text / String(sep); }
inline Ref<StringList, Owner> operator/(String text, Char sep) { return text / String(sep); }
inline Ref<StringList, Owner> operator/(String text, char sep) { return text / String(sep); }
inline String operator*(Ref<StringList> parts, const char* sep) { return parts * String(sep); }
inline String operator*(Ref<StringList> parts, Char sep) { return parts * String(sep); }
inline String operator*(Ref<StringList> parts, char sep) { return parts * String(sep); }

PONA_API int toInt(String s, bool* ok = 0);
PONA_API double toFloat(String s, bool* ok = 0);
PONA_API int64_t toInt64(String s, bool* ok = 0);
PONA_API uint64_t toUInt64(String s, bool* ok = 0);
PONA_API float64_t toFloat64(String s, bool* ok = 0);

PONA_API Char lowerCase(Char ch);
PONA_API Char upperCase(Char ch);
PONA_API String toLower(String s);
PONA_API String toUpper(String s);
PONA_API String stripLeadingSpace(String s);
PONA_API String stripTrailingSpace(String s);
PONA_API uint32_t crc32(String s);

} // namespace pona

#endif // PONA_STRING_HPP
