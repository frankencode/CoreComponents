/*
 * Array.hpp -- handling opaque memory vectors
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_ARRAY_HPP
#define PONA_ARRAY_HPP

#include "Sequence.hpp"
#include "ArrayPolicy.hpp"

namespace pona
{

template<class T, template<class> class P = ArrayPolicy>
class Array: public Sequence<T>
{
public:
	typedef T Element;
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
	
	inline bool def(int i) const { return (0 <= i) && (i < size_); }
	
	inline T get(int i) const {
		assert(def(i));
		return data_[i];
	}
	
	inline void set(int i, T e) {
		assert(def(i));
		data_[i] = e;
	}
	
	inline bool operator< (const Array& b) const { return pona::strcmp(data_, b.data_) <  0; }
	inline bool operator==(const Array& b) const { return pona::strcmp(data_, b.data_) == 0; }
	inline bool operator> (const Array& b) const { return pona::strcmp(data_, b.data_) >  0; }
	inline bool operator!=(const Array& b) const { return pona::strcmp(data_, b.data_) != 0; }
	inline bool operator<=(const Array& b) const { return pona::strcmp(data_, b.data_) <= 0; }
	inline bool operator>=(const Array& b) const { return pona::strcmp(data_, b.data_) >= 0; }
	
	inline T* pointerAt(int i) const {
		assert(def(i));
		return data_ + i;
	}
	
	inline T& at(int i) const {
		assert(def(i));
		return data_[i];
	}
	
	inline T* data() const { return data_; }
	inline operator T*() const { return data_; }
	inline operator bool() const { return data_; }
	
private:
	int size_;
	T* data_;
};

} // namespace pona

#endif // PONA_ARRAY_HPP
