/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_ARRAY_H
#define FLUX_ARRAY_H

#include "strings.h"
#include "containers.h"

namespace flux
{

template<class T>
class Array: public Object
{
public:
	typedef T Item;

	inline static Ref<Array> create(int size = 0) { return new Array(size); }
	inline static Ref<Array> create(int size, T zero) { return new Array(size, zero); }
	inline static Ref<Array> copy(const T *data, int size) { return new Array(data, size); }

	~Array()
	{
		if (size_ > 0) delete[] data_;
	}

	inline Array &operator=(const Array &b)
	{
		if (size_ != b.size_) {
			if (size_ > 0) delete[] data_;
			if (b.size_ > 0) {
				size_ = b.size_;
				data_ = new T[b.size_];
			}
			else {
				size_ = 0;
				data_ = 0;
			}
		}
		memcpy(data_, b.data_, b.size_ * sizeof(T));
		return *this;
	}

	inline void clear(const T &zero = T())
	{
		for (int i = 0, n = size_; i < n; ++i)
			data_[i] = zero;
	}

	inline void reset(int newSize = 0)
	{
		if (size_ != newSize) {
			if (size_ > 0) delete[] data_;
			if (newSize > 0) {
				size_ = newSize;
				data_ = new T[newSize];
			}
			else {
				size_ = 0;
				data_ = 0;
			}
		}
	}

	inline void truncate(int newSize)
	{
		if (newSize <= 0)
			reset();
		else if (newSize < size_)
			size_ = newSize;
	}

	inline int count() const { return size_; }

	inline int first() const { return 0; }
	inline int last() const { return size_ - 1; }

	inline bool has(int i) const {
		return (0 <= i) && (i < size_);
	}

	inline T *pointerAt(int i) const {
		FLUX_ASSERT(has(i));
		return data_ + i;
	}

	inline T &at(int i) const {
		FLUX_ASSERT(has(i));
		return data_[i];
	}

	inline T *data() const { return data_; }
	inline const T *constData() const { return data_; }
	inline operator T*() const { return data_; }

	inline void read(int i, T *data, int size) {
		if (size == 0) return;
		FLUX_ASSERT(has(i));
		FLUX_ASSERT(has(i + size - 1));
		memcpy(data, data_ + i, size * sizeof(T));
	}

	inline void write(int i, const T *data, int size) {
		if (size == 0) return;
		FLUX_ASSERT(has(i));
		FLUX_ASSERT(has(i + size - 1));
		memcpy(data_ + i, data, size * sizeof(T));
	}

	inline Ref<Array> copy() const { return copy(0, size_); }

	inline Ref<Array> copy(int i0, int i1) const {
		if (i0 < 0) i0 = 0;
		if (i0 > size_) i0 = size_;
		if (i1 < 0) i1 = 0;
		if (i1 > size_) i1 = size_;
		return (i0 < i1) ? new Array(data_ + i0, i1 - i0) : new Array();
	}

	inline Ref<Array> head(int n) const { return copy(0, n); }
	inline Ref<Array> tail(int n) const { return copy(size_ - n, size_); }

	inline int find(const T &item) const { return find(0, item); }
	inline int find(const T &item, int i) const {
		while (i < size_)
			if (data_[i++] == item) return i - 1;
		return size_;
	}

	inline bool contains(const T &item) const { return find(item) < size_; }

	inline int replace(const T &oldItem, const T &newItem) {
		int n = 0;
		for (int i = 0; i < size_; ++i) {
			if (data_[i] == oldItem) {
				data_[i] = newItem;
				++n;
			}
		}
		return n;
	}

	inline int find(Array *pattern, int i) const { return find(i, pattern->data(), pattern->count()); }
	inline int find(const T *pattern, int patternSize, int i) const {
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

	inline int contains(Array *pattern) { return contains(pattern->data(), pattern->count()); }
	inline int contains(const T *pattern, int patternSize) { return find(0, pattern, patternSize) != size_; }

protected:
	Array(int size = 0)
		: size_(0),
		  data_(0)
	{
		if (size > 0) {
			size_ = size;
			data_ = new T[size];
		}
	}

	Array(int size, T zero)
		: size_(0),
		  data_(0)
	{
		if (size > 0) {
			size_ = size;
			data_ = new T[size];
			memclr(data_, size * sizeof(T), zero);
		}
	}

	Array(const T *data, int size)
		: size_(0),
		  data_(0)
	{
		if (size > 0) {
			size_ = size;
			data_ = new T[size];
			memcpy(data_, data, size * sizeof(T));
		}
	}

	Array(const Array &b)
		: size_(0),
		  data_(0)
	{
		if (b.size_ > 0) {
			size_ = b.size_;
			data_ = new T[b.size_];
			memcpy(data_, b.data_, b.size_ * sizeof(T));
		}
	}

	int size_;
	T *data_;
};

template<class T>
bool operator==(const Array<T> &a, const Array<T> &b) { return container::compare(a, b) == 0; }

template<class T>
bool operator!=(const Array<T> &a, const Array<T> &b) { return container::compare(a, b) != 0; }

template<class T>
bool operator<(const Array<T> &a, const Array<T> &b) { return container::compare(a, b) < 0; }

template<class T>
bool operator>(const Array<T> &a, const Array<T> &b) { return container::compare(a, b) > 0; }

template<class T>
bool operator<=(const Array<T> &a, const Array<T> &b) { return container::compare(a, b) <= 0; }

template<class T>
bool operator>=(const Array<T> &a, const Array<T> &b) { return container::compare(a, b) >= 0; }

} // namespace flux

#endif // FLUX_ARRAY_H
