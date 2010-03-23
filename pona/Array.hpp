/*
 * Array.hpp -- handling opaque memory vectors
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_ARRAY_HPP
#define PONA_ARRAY_HPP

#include "containers.hpp"
#include "ArrayPolicy.hpp"

namespace pona
{

template<class T, template<class> class P = ArrayPolicy>
class Array: public Sequence<T, int>, public NonCopyable
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
	inline bool empty() const { return size_ == 0; }
	
	inline bool def(int i) const {
		if (i < 0) i += size_;
		return (0 <= i) && (i < size_);
	}
	
	inline T get(int i) const {
		if (i < 0) i += size_;
		check(def(i));
		return data_[i];
	}
	
	inline void set(int i, const T& item) {
		if (i < 0) i += size_;
		check(def(i));
		data_[i] = item;
	}
	
	inline T* pointerAt(int i) const {
		if (i < 0) i += size_;
		check(def(i));
		return data_ + i;
	}
	
	inline T& at(int i) const {
		if (i < 0) i += size_;
		check(def(i));
		return data_[i];
	}
	
	inline int find(const T& item) {
		for (int i = 0; i < size_; ++i)
			if (data_[i] == item) return i;
		return size_;
	}
	
	inline bool contains(const T& item) { return find(item) < size_; }
	
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
	
	inline T* data() const { return data_; }
	inline operator T*() const { return data_; }
	inline operator bool() const { return !empty(); }
	
private:
	int size_;
	T* data_;
};

} // namespace pona

#endif // PONA_ARRAY_HPP
