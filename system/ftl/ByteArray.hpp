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

namespace ftl
{

class ByteArray: public Sequence<char, int>
{
public:
	typedef int Index;
	typedef char Item;
	
	ByteArray()
		: size_(0),
		  data_(const_cast<char*>(""))
	{}
	
	explicit ByteArray(int size)
		: size_(0),
		  data_(const_cast<char*>(""))
	{
		if (size > 0) {
			size_ = size;
			data_ = new char[size + 1];
			data_[size] = 0;
		}
	}
	
	ByteArray(int size, char zero)
		: size_(0),
		  data_(const_cast<char*>(""))
	{
		if (size > 0) {
			size_ = size;
			data_ = new char[size + 1];
			mem::clr(data_, size_, zero);
			data_[size] = 0;
		}
	}
	
	ByteArray(const char* data, int size = -1)
		: size_(0),
		  data_(const_cast<char*>(""))
	{
		if (size < 0) size = str::len(data);
		if (size > 0) {
			size_ = size;
			data_ = new char[size + 1];
			mem::cpy(data_, data, size);
			data_[size] = 0;
		}
	}
	
	ByteArray(const ByteArray& b)
		: size_(0),
		  data_(const_cast<char*>(""))
	{
		if (b.size_ > 0) {
			size_ = b.size_;
			data_ = new char[b.size_ + 1];
			mem::cpy(data_, b.data_, b.size_ + 1);
		}
	}
	
	~ByteArray() {
		if (size_ > 0) delete[] data_;
	}
	
	inline static Ref<ByteArray> empty() { return Default<ByteArray>::instance(); }
	
	inline ByteArray& operator=(const ByteArray& b)
	{
		if (size_ != b.size_) {
			if (size_ > 0) delete[] data_;
			size_ = 0;
			data_ = const_cast<char*>("");
			if (b.size_ > 0) {
				size_ = b.size_;
				data_ = new char[b.size_ + 1];
			}
		}
		if (b.size_ > 0)
			mem::cpy(data_, b.data_, b.size_ + 1);
		return *this;
	}
	
	inline void clear(char zero = '\0')
	{
		mem::clr(data_, size_, zero);
	}
	
	inline void reset(int newSize = 0)
	{
		if (size_ > 0) delete[] data_;
		if (newSize > 0) {
			size_ = newSize;
			data_ = new char[newSize + 1];
			data_[newSize] = 0;
		}
		else {
			size_ = 0;
			data_ = const_cast<char*>("");
		}
	}
	
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
	
	inline int find(int i, Ref<ByteArray> pattern) const { return find(i, pattern->data(), pattern->size()); }
	inline int find(int i, const char* pattern, int patternSize) const {
		if (patternSize == 0) return size_;
		for (int j = i, k = 0; j < size_;) {
			if (data_[j++] == pattern[k]) {
				++k;
				if (k == patternSize)
					return j - k;
				k = 0;
			}
		}
		return size_;
	}
	
	inline int contains(Ref<ByteArray> pattern) { return contains(pattern->data(), pattern->size()); }
	inline int contains(const char* pattern, int patternSize) { return find(0, pattern, patternSize) != size_; }
	
	inline Ref<ByteArray, Owner> copy() const { return new ByteArray(*this); }
	
	inline Ref<ByteArray, Owner> copy(int i0, int i1) const {
		if (i0 < 0) i0 = 0;
		if (i0 > size_) i0 = size_;
		if (i1 < 0) i1 = 0;
		if (i1 > size_) i1 = size_;
		return (i0 < i1) ? new ByteArray(data_ + i0, i1 - i0) : new ByteArray();
	}
	
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
	
	inline void truncate(int newSize) {
		if (newSize <= 0) {
			reset();
		}
		else if (newSize < size_) {
			size_ = newSize;
			data_[newSize] = 0;
		}
	}
	
	inline Ref<ByteArray, Owner> head(int n) const { return copy(0, n); }
	inline Ref<ByteArray, Owner> tail(int n) const { return copy(size_ - n, size_); }
	
	inline char* data() const { return data_; }
	inline const char* constData() const { return data_; }
	inline char& operator[](int i) { return at(i); }
	inline char operator[](int i) const { return get(i); }
	inline operator char*() const { return data_; }
	inline operator bool() const { return !isEmpty(); }
	
protected:
	int size_;
	char* data_;
};

} // namespace ftl

#endif // FTL_BYTEARRAY_HPP
