#ifndef PONA_USTRING_HPP
#define PONA_USTRING_HPP

#include "atoms"
#include "Utf8Iterator.hpp"
#include "Array.hpp"
#include "List.hpp"

namespace pona
{

class String;
class Variant;

typedef List<String> UStringList;
typedef Array<char, DeepCopyZeroTerminatedArray> UStringMedia;

class String: public Ref<UStringMedia, Owner>
{
public:
	typedef UStringMedia Media;
	typedef Ref<Media, Owner> Super;
	// typedef uchar_t Element;
	typedef Utf8Iterator Index;
	
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
		if (size < 0) size = pona::strlen(data);
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
	String(Ref<UStringList, GetAndSetPolicy> parts, const char* glue = "") {
		assign(parts, glue);
	}
	
	// initialize string from a shallow copy of another string
	String(const String& b): Super(b.Super::get()) {}
	
	// initialize string from a variant
	String(const Variant& b);
	
	// assign a copy of an UTF8 encoded string
	inline String& operator=(const char* data) {
		int size = pona::strlen(data);
		validate(data, size);
		assign(data, size);
		return *this;
	}
	
	// assign a concatenation of a string list
	template<template<class> class GetAndSetPolicy>
	inline String& operator=(Ref<UStringList, GetAndSetPolicy> parts) {
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
	inline Index last() const { return media()->empty() ? Index() : eoi() - 1; }
	inline Index eoi() const { return media()->empty() ? Index() : Index(media()->data(), media()->data() + media()->size()); }
	
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
	Ref<UStringList, Owner> split(const char* pattern) const;
	
	inline Index find(const char* pattern) const { return find(first(), pattern); }
	inline bool contains(const char* pattern) const { return find(first(), pattern).valid(); }
	
	inline bool operator< (const String& b) const { return pona::strcmp((*this)->data(), b->data()) <  0; }
	inline bool operator==(const String& b) const { return pona::strcmp((*this)->data(), b->data()) == 0; }
	inline bool operator> (const String& b) const { return pona::strcmp((*this)->data(), b->data()) >  0; }
	inline bool operator!=(const String& b) const { return pona::strcmp((*this)->data(), b->data()) != 0; }
	inline bool operator<=(const String& b) const { return pona::strcmp((*this)->data(), b->data()) <= 0; }
	inline bool operator>=(const String& b) const { return pona::strcmp((*this)->data(), b->data()) >= 0; }
	
	int toInt(bool* ok = 0);
	double toFloat(bool* ok = 0);
	int64_t toInt64(bool* ok = 0);
	uint64_t toUInt64(bool* ok = 0);
	float64_t toFloat64(bool* ok = 0);
	
	String toLower() const;
	String toUpper() const;
	String stripLeadingSpace() const;
	String stripTrailingSpace() const;
	
private:
	// validate input string and throw StreamIoException if not a valid UTF8 string
	static void validate(const char* data, int size = -1);
	
	inline void assign(const char* data, int size = -1) {
		if (size < 0) size = pona::strlen(data);
		set(new Media(data, size));
	}
	void assign(Ref<UStringList> parts, const char* glue = "");
};

inline bool operator< (const char* a, const String& b) { return pona::strcmp(a, b->data()) <  0; }
inline bool operator==(const char* a, const String& b) { return pona::strcmp(a, b->data()) == 0; }
inline bool operator> (const char* a, const String& b) { return pona::strcmp(a, b->data()) >  0; }
inline bool operator!=(const char* a, const String& b) { return pona::strcmp(a, b->data()) != 0; }
inline bool operator<=(const char* a, const String& b) { return pona::strcmp(a, b->data()) <= 0; }
inline bool operator>=(const char* a, const String& b) { return pona::strcmp(a, b->data()) >= 0; }

inline bool operator< (char* a, const String& b) { return pona::strcmp(a, b->data()) <  0; }
inline bool operator==(char* a, const String& b) { return pona::strcmp(a, b->data()) == 0; }
inline bool operator> (char* a, const String& b) { return pona::strcmp(a, b->data()) >  0; }
inline bool operator!=(char* a, const String& b) { return pona::strcmp(a, b->data()) != 0; }
inline bool operator<=(char* a, const String& b) { return pona::strcmp(a, b->data()) <= 0; }
inline bool operator>=(char* a, const String& b) { return pona::strcmp(a, b->data()) >= 0; }

inline bool operator< (const String& a, const char* b) { return pona::strcmp(a->data(), b) <  0; }
inline bool operator==(const String& a, const char* b) { return pona::strcmp(a->data(), b) == 0; }
inline bool operator> (const String& a, const char* b) { return pona::strcmp(a->data(), b) >  0; }
inline bool operator!=(const String& a, const char* b) { return pona::strcmp(a->data(), b) != 0; }
inline bool operator<=(const String& a, const char* b) { return pona::strcmp(a->data(), b) <= 0; }
inline bool operator>=(const String& a, const char* b) { return pona::strcmp(a->data(), b) >= 0; }

inline bool operator< (const String& a, char* b) { return pona::strcmp(a->data(), b) <  0; }
inline bool operator==(const String& a, char* b) { return pona::strcmp(a->data(), b) == 0; }
inline bool operator> (const String& a, char* b) { return pona::strcmp(a->data(), b) >  0; }
inline bool operator!=(const String& a, char* b) { return pona::strcmp(a->data(), b) != 0; }
inline bool operator<=(const String& a, char* b) { return pona::strcmp(a->data(), b) <= 0; }
inline bool operator>=(const String& a, char* b) { return pona::strcmp(a->data(), b) >= 0; }

} // namespace pona

#endif // PONA_USTRING_HPP
