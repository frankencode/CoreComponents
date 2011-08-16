/*
 * Array.hpp -- opaque memory vectors
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_ARRAY_HPP
#define FTL_ARRAY_HPP

#include "generics.hpp"
#include "strings.hpp"
#include "ArrayPolicy.hpp"

namespace ftl
{

template<class T, template<class> class P = ArrayPolicy>
class Array: public Sequence<T, int>
{
public:
	typedef int Index;
	typedef T Item;
	typedef P<T> Policy;
	
	Array() { Policy::initEmpty(data_, size_); }
	Array(int size) { Policy::alloc(data_, size_, size); }
	Array(int size, T zero) {
		Policy::alloc(data_, size_, size);
		Policy::clear(data_, size_, zero);
	}
	Array(const T* data, int size) {
		Policy::initEmpty(data_, size_);
		Policy::assign(data_, size_, data, size);
	}
	Array(const Array& b) {
		Policy::initEmpty(data_, size_);
		Policy::assign(data_, size_, b.data_, b.size_);
	}
	
	~Array() { Policy::free(data_, size_); }
	
	inline Array& operator=(const Array& b) {
		Policy::assign(data_, size_, b.data_, b.size_);
		return *this;
	}
	
	inline void clear(const T& zero = T()) {
		Policy::clear(data_, size_, zero);
	}
	
	inline void reset() {
		Policy::free(data_, size_);
	}
	
	inline void reset(int newSize) {
		Policy::free(data_, size_);
		Policy::alloc(data_, size_, newSize);
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
	
	inline T get(int i) const {
		check(has(i));
		if (i < 0) i += size_;
		return data_[i];
	}
	
	inline void set(int i, const T& item) {
		check(has(i));
		if (i < 0) i += size_;
		data_[i] = item;
	}
	
	inline T* pointerAt(int i) const {
		check(has(i));
		if (i < 0) i += size_;
		return data_ + i;
	}
	
	inline T& at(int i) const {
		check(has(i));
		if (i < 0) i += size_;
		return data_[i];
	}
	
	inline int find(const T& item) const { return find(0, item); }
	inline int find(int i, const T& item) const {
		while (i < size_)
			if (data_[i++] == item) return i - 1;
		return size_;
	}
	
	inline bool contains(const T& item) const { return find(item) < size_; }
	
	inline int replace(const T& oldItem, const T& newItem) {
		int n = 0;
		for (int i = 0; i < size_; ++i) {
			if (data_[i] == oldItem) {
				data_[i] = newItem;
				++n;
			}
		}
		return n;
	}
	
	inline int find(int i, Ref<Array> pattern) const { return find(i, pattern->data(), pattern->size()); }
	inline int find(int i, const T* pattern, int patternSize) const {
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
	
	inline int contains(Ref<Array> pattern) { return contains(pattern->data(), pattern->size()); }
	inline int contains(const T* pattern, int patternSize) { return find(0, pattern, patternSize) != size_; }
	
	inline Ref<Array, Owner> copy() const { return copy(0, size_); }
	
	inline Ref<Array, Owner> copy(int i0, int i1) const {
		if (i0 < 0) i0 = 0;
		if (i0 > size_) i0 = size_;
		if (i1 < 0) i1 = 0;
		if (i1 > size_) i1 = size_;
		return (i0 < i1) ? new Array(data_ + i0, i1 - i0) : new Array();
	}
	
	inline void read(int i, T* data, int size) {
		if (size == 0) return;
		check(has(i));
		check(has(i + size - 1));
		mem::cpy(data, data_ + i, size * sizeof(T));
	}
	
	inline void write(int i, const T* data, int size) {
		if (size == 0) return;
		check(has(i));
		check(has(i + size - 1));
		mem::cpy(data_ + i, data, size * sizeof(T));
	}
	
	inline void truncate(int newSize) {
		if (newSize < 0) newSize = 0;
		if (newSize < size_)
			Policy::truncate(data_, size_, newSize);
	}
	
	inline Ref<Array, Owner> head(int n) const {
		if (n > size_) n = size_;
		return copy(0, n);
	}
	inline Ref<Array, Owner> tail(int n) const {
		if (n > size_) n = size_;
		return copy(size_ - n, size_);
	}
	
	inline T* data() const { return data_; }
	inline const T* constData() const { return data_; }
	inline operator T*() const { return data_; }
	inline operator bool() const { return !isEmpty(); }
	
private:
	int size_;
	T* data_;
};

typedef Array<char, DeepCopyZeroTerminatedArray> ByteArray;

} // namespace ftl

#endif // FTL_ARRAY_HPP
