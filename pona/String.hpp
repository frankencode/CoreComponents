/*
 * String.hpp -- UTF8 strings
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_STRING_HPP
#define PONA_STRING_HPP

#include "atoms"
#include "strings.hpp"
#include "Utf8Walker.hpp"
#include "Array.hpp"
#include "List.hpp"

namespace pona
{

class String;
class Variant;

typedef List<String> StringList;

class String: public Ref<StringMedia, Owner>
{
public:
	typedef StringMedia Media;
	typedef Ref<Media, Owner> Super;
	typedef Utf8Walker Index;
	typedef uchar_t Item;
	
	// initialize empty string
	String(): Super(new Media) {}
	
	// initialize string with defined size but undefined content
	explicit String(int size): Super(new Media(size)) {}
	
	// initialize string with defined size and defined content
	explicit String(int size, char zero): Super(new Media(size, zero)) {
		check(0 <= zero);
	}
	
	// initialize string by deep-copying an UTF8 encoded string
	String(const char* data, int size = -1) {
		if (size < 0) size = str::len(data);
		validate(data, size);
		assign(data, size);
	}
	
	// initialize string by deep-copying an UTF8 encoded sub-string
	template<template<class> class GetAndSetPolicy>
	explicit String(Ref<Media, GetAndSetPolicy> media, int i = 0, int n = -1) {
		if (n < 0) n = media->size();
		const char* data = media->pointerAt(i);
		int size = media->pointerAt(i + n - 1) - media->pointerAt(i) + 1;
		validate(data, size);
		assign(data, size);
	}
	
	// initialize string by deep-copying an index range
	String(const Index& index0, const Index& index1) {
		assign(index0.pos(), index1.pos() - index0.pos());
	}
	
	// initialize string by concatenating a string list
	template<template<class> class GetAndSetPolicy>
	String(Ref<StringList, GetAndSetPolicy> parts, const char* glue = "") {
		assign(parts, glue);
	}
	
	// initialize string from a shallow copy of another string
	String(const String& b): Super(b.Super::get()) {}
	
	// initialize string from a variant
	String(const Variant& b);
	
	// assign a copy of an UTF8 encoded string
	inline String& operator=(const char* data) {
		int size = str::len(data);
		validate(data, size);
		assign(data, size);
		return *this;
	}
	
	// assign a concatenation of a string list
	template<template<class> class GetAndSetPolicy>
	inline String& operator=(Ref<StringList, GetAndSetPolicy> parts) {
		assign(parts);
		return *this;
	}
	
	// assign a shallow copy of another string
	inline String& operator=(const String& b) {
		Super::set(b.media());
		return *this;
	}
	
	// return a deep copy of this string
	String deepCopy() const;
	
	// provide access to the shared media
	inline Ref<Media> media() const { return Super::get(); }
	inline operator char*() const { return media()->data(); }
	
	inline Index first() const { return media()->empty() ? Index() : Index(media()->data()); }
	inline Index last() const { return media()->empty() ? Index() : end() - 1; }
	inline Index end() const { return media()->empty() ? Index() : Index(media()->data(), media()->data() + media()->size()); }
	
	inline bool empty() const { return media()->empty(); }
	
	inline bool def(const Index& index) const {
		check(!media()->empty());
		check(index.data() == media()->data());
		return index.valid();
	}
	inline uchar_t get(const Index& index) const {
		check(!media()->empty());
		check(index.data() == media()->data());
		return index.getChar();
	}
	inline uchar_t operator[](const Index& index) { return get(index); }
	
	inline void validate() const { validate(media()->data(), media()->size()); }
	inline bool valid() const {
		try { validate(); }
		catch (StreamIoException&) { return false; }
		return true;
	}
	
	Index find(const Index& index, const char* pattern) const;
	Ref<StringList, Owner> split(const char* pattern) const;
	
	inline Index find(const char* pattern) const { return find(first(), pattern); }
	inline bool contains(const char* pattern) const { return find(first(), pattern).valid(); }
	inline String replace(const char* pattern, const char* replacement) {
		Ref<StringList, Owner> parts = split(pattern);
		return String(parts, replacement);
	}
	
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
	
private:
	// validate input string and throw StreamIoException if not a valid UTF8 string
	static void validate(const char* data, int size = -1);
	
	inline void assign(const char* data, int size = -1) {
		if (size < 0) size = str::len(data);
		set(new Media(data, size));
	}
	void assign(Ref<StringList> parts, const char* glue = "");
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

} // namespace pona

#endif // PONA_STRING_HPP
