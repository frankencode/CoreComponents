/*
 * Array.hpp -- opaque memory vector
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_VECTOR_HPP
#define PONA_VECTOR_HPP

#include "atoms"
#include "RandomAccessMedia.hpp"

namespace pona
{

template<class T>
class Array: public RandomAccessMedia<T>
{
public:
	typedef T Element;
	
	Array()
		: size_(0),
		  buf_(0),
		  owner_(false)
	{}
	
	Array(int size)
		: size_(size),
		  buf_((size > 0) ? new T[size] : 0),
		  owner_(true)
	{
		if (PONA_IS_ATOMIC(T))
			clear();
	}
	
	template<class T2>
	Array(T2* buf)
		: size_(0),
		  buf_(0),
		  owner_(false)
	{
		size_ = 0;
		if (buf) {
			while (buf[size_]) ++size_;
			buf_ = new T[size_];
			owner_ = true;
			for (int i = 0; i < size_; ++i)
				buf_[i] = buf[i];
		}
	}
	
	template<class T2>
	Array(T2* buf, int size)
		: size_(size),
		  buf_((size > 0) ? new T[size] : 0),
		  owner_(size > 0)
	{
		for (int i = 0; i < size; ++i)
			buf_[i] = buf[i];
	}
	
	~Array()
	{
		if (owner_) {
			if (buf_) {
				delete[] buf_;
				buf_ = 0;
			}
		}
	}
	
	Array(const Array& b)
	{
		*this = b;
	}
	
	void liberate() { owner_ = false; }
	
	inline const Array& operator=(const Array& b)
	{
		if (size_ != b.size_) {
			if (owner_)
				if (buf_)
					delete[] buf_;
			size_ = b.size_;
			owner_ = (b.size_ > 0);
			buf_ = (b.size_ > 0) ? new T[b.size_] : 0;
		}
		
		for (int i = 0; i < size_; ++i)
			buf_[i] = b.buf_[i];
			
		return *this;
	}
	
	inline void wrap(T* buf, int size)
	{
		if (owner_)
			if (buf_)
				delete[] buf_;
		owner_ = false;
		buf_ = buf;
		size_ = size;
	}
	
	inline void take(T* buf, int size)
	{
		wrap(buf, size);
		owner_ = true;
	}
	
	inline void reset(int newSize)
	{
		take((newSize > 0) ? new T[newSize] : 0, newSize);
		if (PONA_IS_ATOMIC(T))
			clear();
	}
	
	inline void clear(const T& value = T())
	{
		for (int i = 0; i < size_; ++i)
			buf_[i] = value;
	}
	
	inline int size() const { return size_; }
	inline int length() const { return size_; }
	
	inline void set(int i, T e)
	{
		assert((0 <= i) && (i < size_));
		buf_[i] = e;
	}
	
	inline T get(int i) const
	{
		assert((0 <= i) && (i < size_));
		return buf_[i];
	}
	
	inline T* at(int i)
	{
		assert((0 <= i) && (i < size_));
		return buf_ + i;
	}
	
	inline bool operator<(const Array& b) const
	{
		int n = (size_ < b.size_) ? size_ : b.size_;
		int i = 0;
		while (i < n) {
			T c1 = get(i);
			T c2 = b.get(i);
			if ((c1 < c2) || (c2 < c1))
				return c1 < c2;
			++i;
		}
		return size_ < b.size_;
	}
	
	inline bool operator==(const Array& b) const
	{
		bool equal = (size_ == b.size_);
		for (int i = 0; equal && (i < size_); ++i)
			equal = (buf_[i] == b.buf_[i]);
		return equal;
	}
	
	inline bool operator>(const Array& b) const { return b < *this; }
	inline bool operator!=(const Array& b) const { return !(*this == b); }
	inline bool operator<=(const Array& b) const { return (*this < b) || (*this == b); }
	inline bool operator>=(const Array& b) const { return (b < *this) || (*this == b); }
	
	inline operator bool() const { return buf_; }
	inline operator T*() const { return buf_; }

private:
	int size_;
	T* buf_;
	bool owner_;
};

} // namespace pona

#endif // PONA_VECTOR_HPP
