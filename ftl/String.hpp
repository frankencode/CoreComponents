/*
 * String.hpp -- UTF-8 strings
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_STRING_HPP
#define FTL_STRING_HPP

#include "atoms"
#include "strings.hpp"
#include "Utf8Walker.hpp"
#include "Character.hpp"
#include "List.hpp"
#include "Singleton.hpp"

namespace ftl
{

class String;
class Format;
class Variant;
class Path;

typedef List<String> StringList;

class String: public Ref<ByteArray, Owner>
{
public:
	typedef Ref<ByteArray, Owner> Super;
	typedef uchar_t Item;
	
	// initialize empty string
	String(): Super(ByteArray::empty()) {}
	
	// initialize string by a given string bytes
	String(const Ref<ByteArray, Owner>& bytes): Super(bytes) {}
	
	// initialize string from a shallow copy of another string
	String(const String& b): Super(b.Super::get()) {}
	
	// helper constructors
	String(const Format& b);
	String(const Variant& b);
	String(const Path& b);
	
	// initialize string by deep-copying a byte array
	String(const char* data) { assign(data, str::len(data)); }
	String(const char* data, int size) { assign(data, size); }
	
	// alternative construction methods
	static inline String uninitialized(int size) { return String(size); }
	static inline String initialized(int size, char zero) { return String(size, zero); }
	
	// conversion from and to UTF-16
	static String fromUtf16(const void* data, int size = -1, int endian = localEndian());
	bool toUtf16(void* buf, int* size);
	Ref<ByteArray, Owner> toUtf16(int endian = localEndian());
	
	// counterpart to StringList::join()
	static inline String join(Ref<StringList> parts, const char* sep = "") {
		String s;
		s.assign(parts, sep);
		return s;
	}
	
	// assign a shallow copy of another string
	inline String& operator=(const String& b) {
		Super::set(b.bytes());
		return *this;
	}
	
	// assign a deep copy of a byte array
	inline String& operator=(const char* data) {
		String b(data);
		Super::set(b.bytes());
		return *this;
	}
	
	// return a deep copy of this string
	String copy() const;
	inline String copy(int i0, int i1) const { return character()->copy(i0, i1); }
	inline String head(int n) const { return character()->copy(0, n); }
	inline String tail(int n) const { return character()->copy(length() - n, n); }
	
	// provide access to the shared bytes
	inline Ref<ByteArray> bytes() const { return Super::get(); }
	inline operator char*() const { return bytes()->data(); }
	inline char* utf8() const { return bytes()->data(); }
	inline int encodedSize() const { return bytes()->size(); }
	
	inline bool isEmpty() const { return bytes()->isEmpty(); }
	
	inline Ref<Character> character() const {
		if (!character_)
			character_ = new Character(Super::get());
		return character_;
	}
	
	inline int length() const { return character()->length(); }
	
	inline bool has(int i) const { return character()->has(i); }
	inline uchar_t get(int i) const { return character()->get(i); }
	inline uchar_t operator[](int i) const { return get(i); }
	
	inline void validate() const { validate(bytes()->data(), bytes()->size()); }
	bool valid() const;
	
	int find(int i, const char* pattern) const;
	Ref<StringList, Owner> split(const char* pattern) const;
	
	inline int find(const char* pattern) const { return find(0, pattern); }
	inline bool contains(const char* pattern) const { return bytes()->find(0, pattern, str::len(pattern)) < bytes()->size(); }
	inline String replace(const char* pattern, const char* replacement) const { return split(pattern)->join(replacement); }
	void replaceInsitu(const char* pattern, const char* replacement);
	
	inline bool operator< (const String& b) const { return str::cmp((*this)->data(), b->data()) <  0; }
	inline bool operator==(const String& b) const { return str::cmp((*this)->data(), b->data()) == 0; }
	inline bool operator> (const String& b) const { return str::cmp((*this)->data(), b->data()) >  0; }
	inline bool operator!=(const String& b) const { return str::cmp((*this)->data(), b->data()) != 0; }
	inline bool operator<=(const String& b) const { return str::cmp((*this)->data(), b->data()) <= 0; }
	inline bool operator>=(const String& b) const { return str::cmp((*this)->data(), b->data()) >= 0; }
	
	int toInt(bool* ok = 0) const;
	double toFloat(bool* ok = 0) const;
	int64_t toInt64(bool* ok = 0) const;
	uint64_t toUInt64(bool* ok = 0) const;
	float64_t toFloat64(bool* ok = 0) const;
	
	String toLower() const;
	String toUpper() const;
	String stripLeadingSpace() const;
	String stripTrailingSpace() const;
	String trimmed() const;
	String stripTags() const;
	String simplified() const;
	String normalized(bool nameCase = true) const;
	
	void expandInsitu();
	String expand() const;
	
protected:
	// initialize string with defined size but undefined content
	explicit String(int size): Super(new ByteArray(size)) {}
	
	// initialize string with defined size and defined content
	explicit String(int size, char zero): Super(new ByteArray(size, zero)) {
		FTL_CHECK(0 <= zero);
	}
	
	// validate input string and throw StreamIoException if not a valid UTF-8 string
	static void validate(const char* data, int size = -1);
	
	inline void assign(const char* data, int size = -1) {
		if (size < 0) size = str::len(data);
		set(new ByteArray(data, size));
	}
	void assign(Ref<StringList> parts, const char* sep = "");
	
	mutable Ref<Character, Owner> character_;
};

inline bool operator< (const char* a, const String& b) { return str::cmp(a, b->data()) <  0; }
inline bool operator==(const char* a, const String& b) { return str::cmp(a, b->data()) == 0; }
inline bool operator> (const char* a, const String& b) { return str::cmp(a, b->data()) >  0; }
inline bool operator!=(const char* a, const String& b) { return str::cmp(a, b->data()) != 0; }
inline bool operator<=(const char* a, const String& b) { return str::cmp(a, b->data()) <= 0; }
inline bool operator>=(const char* a, const String& b) { return str::cmp(a, b->data()) >= 0; }

inline bool operator< (char* a, const String& b) { return str::cmp(a, b->data()) <  0; }
inline bool operator==(char* a, const String& b) { return str::cmp(a, b->data()) == 0; }
inline bool operator> (char* a, const String& b) { return str::cmp(a, b->data()) >  0; }
inline bool operator!=(char* a, const String& b) { return str::cmp(a, b->data()) != 0; }
inline bool operator<=(char* a, const String& b) { return str::cmp(a, b->data()) <= 0; }
inline bool operator>=(char* a, const String& b) { return str::cmp(a, b->data()) >= 0; }

inline bool operator< (const String& a, const char* b) { return str::cmp(a->data(), b) <  0; }
inline bool operator==(const String& a, const char* b) { return str::cmp(a->data(), b) == 0; }
inline bool operator> (const String& a, const char* b) { return str::cmp(a->data(), b) >  0; }
inline bool operator!=(const String& a, const char* b) { return str::cmp(a->data(), b) != 0; }
inline bool operator<=(const String& a, const char* b) { return str::cmp(a->data(), b) <= 0; }
inline bool operator>=(const String& a, const char* b) { return str::cmp(a->data(), b) >= 0; }

inline bool operator< (const String& a, char* b) { return str::cmp(a->data(), b) <  0; }
inline bool operator==(const String& a, char* b) { return str::cmp(a->data(), b) == 0; }
inline bool operator> (const String& a, char* b) { return str::cmp(a->data(), b) >  0; }
inline bool operator!=(const String& a, char* b) { return str::cmp(a->data(), b) != 0; }
inline bool operator<=(const String& a, char* b) { return str::cmp(a->data(), b) <= 0; }
inline bool operator>=(const String& a, char* b) { return str::cmp(a->data(), b) >= 0; }

} // namespace ftl

#endif // FTL_STRING_HPP
