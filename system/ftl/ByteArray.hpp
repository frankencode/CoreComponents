/*
 * ByteArray.hpp -- an array of bytes
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_BYTEARRAY_HPP
#define FTL_BYTEARRAY_HPP

#include "generics.hpp"
#include "strings.hpp"
#include "Default.hpp"
#include "List.hpp"

namespace ftl
{

class Character;
class String;
typedef List<String> StringList;

class ByteArray: public Sequence<char, int>
{
public:
	typedef int Index;
	typedef char Item;
	
	explicit ByteArray(int size = 0);
	ByteArray(int size, char zero);
	ByteArray(const char* data, int size = -1);
	ByteArray(const ByteArray& b);
	~ByteArray();
	
	inline static Ref<ByteArray> empty() { return Default<ByteArray>::instance(); }
	
	ByteArray& operator=(const ByteArray& b);
	
	void clear(char zero = '\0');
	void reset(int newSize = 0);
	void truncate(int newSize);
	
	Ref<Character> chars() const;
	
	inline int size() const { return size_; }
	inline int length() const { return size_; }
	inline bool isEmpty() const { return size_ == 0; }
	
	inline int first() const { return 0; }
	inline int last() const { return size_ - 1; }
	
	static inline int ill() { return -1; }
	static inline bool ill(int i) { return i < 0; }
	
	inline bool has(int i) const {
		if (i < 0) i += size_;
		return (0 <= i) && (i < size_);
	}
	
	inline char get(int i) const {
		FTL_ASSERT(has(i));
		if (i < 0) i += size_;
		return data_[i];
	}
	
	inline void set(int i, const char& item) {
		FTL_ASSERT(has(i));
		if (i < 0) i += size_;
		data_[i] = item;
	}
	
	inline char* pointerAt(int i) const {
		FTL_ASSERT(has(i));
		if (i < 0) i += size_;
		return data_ + i;
	}
	
	inline char& at(int i) const {
		FTL_ASSERT(has(i));
		if (i < 0) i += size_;
		return data_[i];
	}
	
	inline char* data() const { return data_; }
	inline const char* constData() const { return data_; }
	inline char& operator[](int i) { return at(i); }
	inline char operator[](int i) const { return get(i); }
	inline operator char*() const { return data_; }
	inline operator bool() const { return !isEmpty(); }
	
	inline void read(int i, char* data, int size) {
		if (size == 0) return;
		FTL_ASSERT(has(i));
		FTL_ASSERT(has(i + size - 1));
		mem::cpy(data, data_ + i, size);
	}
	
	inline void write(int i, const char* data, int size) {
		if (size == 0) return;
		FTL_ASSERT(has(i));
		FTL_ASSERT(has(i + size - 1));
		mem::cpy(data_ + i, data, size);
	}
	
	inline Ref<ByteArray, Owner> copy() const { return new ByteArray(*this); }
	
	inline Ref<ByteArray, Owner> copy(int i0, int i1) const {
		if (i0 < 0) i0 = 0;
		if (i0 > size_) i0 = size_;
		if (i1 < 0) i1 = 0;
		if (i1 > size_) i1 = size_;
		return (i0 < i1) ? new ByteArray(data_ + i0, i1 - i0) : new ByteArray();
	}
	
	inline Ref<ByteArray, Owner> head(int n) const { return copy(0, n); }
	inline Ref<ByteArray, Owner> tail(int n) const { return copy(size_ - n, size_); }
	
	inline int find(const char& item) const { return find(0, item); }
	inline int find(int i, const char& item) const {
		while (i < size_)
			if (data_[i++] == item) return i - 1;
		return size_;
	}
	
	inline bool contains(const char& item) const { return find(item) < size_; }
	
	inline int replace(const char& oldItem, const char& newItem) {
		int n = 0;
		for (int i = 0; i < size_; ++i) {
			if (data_[i] == oldItem) {
				data_[i] = newItem;
				++n;
			}
		}
		return n;
	}
	
	int find(int i, const char* pattern) const;
	inline int find(int i, Ref<ByteArray> pattern) const { return find(i, pattern->data_); }
	inline int find(const char* pattern) const { return find(0, pattern); }
	
	inline int contains(Ref<ByteArray> pattern) { return contains(pattern->data()); }
	inline int contains(const char* pattern) { return find(0, pattern) != size_; }
	
	static Ref<ByteArray, Owner> join(Ref<StringList> parts, const char* sep = "");
	Ref<StringList, Owner> split(const char* pattern) const;
	
	void replaceInsitu(const char* pattern, const char* replacement);
	Ref<ByteArray, Owner> replace(const char* pattern, const char* replacement) const;
	
	int toInt(bool* ok = 0) const;
	double toFloat(bool* ok = 0) const;
	int64_t toInt64(bool* ok = 0) const;
	uint64_t toUInt64(bool* ok = 0) const;
	float64_t toFloat64(bool* ok = 0) const;
	
	Ref<ByteArray> toLowerInsitu();
	Ref<ByteArray> toUpperInsitu();
	inline Ref<ByteArray, Owner> toLower() const { return copy()->toLowerInsitu(); }
	inline Ref<ByteArray, Owner> toUpper() const { return copy()->toUpperInsitu(); }
	
	Ref<ByteArray> expandInsitu();
	inline Ref<ByteArray, Owner> expand() const { return copy()->expandInsitu(); }
	
	Ref<ByteArray, Owner> stripLeadingSpace() const;
	Ref<ByteArray, Owner> stripTrailingSpace() const;
	Ref<ByteArray, Owner> trimmed() const;
	Ref<ByteArray, Owner> stripTags() const;
	Ref<ByteArray, Owner> simplified() const;
	Ref<ByteArray, Owner> normalized(bool nameCase = true) const;
	
	void checkUtf8() const;
	
	static Ref<ByteArray, Owner> fromUtf16(const void* data, int size = -1, int endian = localEndian());
	bool toUtf16(void* buf, int* size);
	Ref<ByteArray, Owner> toUtf16(int endian = localEndian());
	
protected:
	int size_;
	char* data_;
	mutable Ref<Character, Owner> chars_;
};

} // namespace ftl

#endif // FTL_BYTEARRAY_HPP
