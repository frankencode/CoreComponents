/*
 * String.hpp -- UTF8 strings
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
#include "Array.hpp"
#include "List.hpp"

namespace ftl
{

class String;
class Format;
class Variant;
class Path;

typedef List<String> StringList;

class String: public Ref<ByteArray, Owner>, public Sequence<uchar_t, Utf8Walker>
{
public:
	typedef ByteArray Media;
	typedef Ref<Media, Owner> Super;
	typedef Utf8Walker Index;
	typedef uchar_t Item;
	
	// initialize empty string
	String(): Super(new Media) {}
	
	// initialize string by a given string media
	String(const Ref<Media, Owner>& media): Super(media) {}
	
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
	static String fromUtf16(const void* data, int size = -1);
	Ref<ByteArray, Owner> toUtf16();
	
	// counterpart to StringList::join()
	static inline String join(Ref<StringList> parts, const char* sep = "") {
		String s;
		s.assign(parts, sep);
		return s;
	}
	
	// assign a shallow copy of another string
	inline String& operator=(const String& b) {
		Super::set(b.media());
		return *this;
	}
	
	// assign a deep copy of a byte array
	inline String& operator=(const char* data) {
		String b(data);
		Super::set(b.media());
		return *this;
	}
	
	// return a deep copy of this string
	String copy() const;
	String copy(const Index& index0, const Index& index1) const;
	inline String head(int n) const { return copy(first(), first() + n); }
	inline String tail(int n) const { return copy(end() - n, end()); }
	
	// provide access to the shared media
	inline Ref<Media> media() const { return Super::get(); }
	inline operator char*() const { return media()->data(); }
	inline char* utf8() const { return media()->data(); }
	inline int encodedSize() const { return media()->size(); }
	
	inline Index first() const { return media()->isEmpty() ? Index() : Index(media()->data()); }
	inline Index last() const { return media()->isEmpty() ? Index() : end() - 1; }
	inline Index end() const { return media()->isEmpty() ? Index() : Index(media()->data(), media()->data() + media()->size()); }
	inline int length() const { return end() - first(); }
	
	inline bool isEmpty() const { return media()->isEmpty(); }
	
	static inline int ill() { return Index(); }
	
	inline bool def(Index index) const {
		check(index.data() == media()->data());
		return index.valid();
	}
	inline uchar_t get(Index index) const {
		check(!media()->isEmpty());
		check(index.data() == media()->data());
		return index.getChar();
	}
	inline uchar_t operator[](const Index& index) { return get(index); }
	
	inline void validate() const { validate(media()->data(), media()->size()); }
	bool valid() const;
	
	Index find(const Index& index, const char* pattern) const;
	Ref<StringList, Owner> split(const char* pattern) const;
	
	inline Index find(const char* pattern) const { return find(first(), pattern); }
	inline bool contains(const char* pattern) const { return find(first(), pattern).valid(); }
	inline String replace(const char* pattern, const char* replacement) { return split(pattern)->join(replacement); }
	
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
	
protected:
	// initialize string with defined size but undefined content
	explicit String(int size): Super(new Media(size)) {}
	
	// initialize string with defined size and defined content
	explicit String(int size, char zero): Super(new Media(size, zero)) {
		check(0 <= zero);
	}
	
	// validate input string and throw StreamIoException if not a valid UTF8 string
	static void validate(const char* data, int size = -1);
	
	inline void assign(const char* data, int size = -1) {
		if (size < 0) size = str::len(data);
		set(new Media(data, size));
	}
	void assign(Ref<StringList> parts, const char* sep = "");
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
