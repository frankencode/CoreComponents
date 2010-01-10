/*
 * Array.hpp -- opaque memory vector
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_ARRAY_HPP
#define PONA_ARRAY_HPP

#include "Sequence.hpp"

namespace pona
{

template<class T>
class AbstractArray: public Sequence<T>
{
public:
	AbstractArray()
		: size_(0),
		  buf_(0)
	{}
	
	AbstractArray(T* buf, int size)
		: size_(size),
		  buf_(buf)
	{}
	
	inline bool def(int i) const { return (0 <= i) && (i < size_); }
	
	inline T get(int i) const
	{
		assert(def(i));
		return buf_[i];
	}
	
	inline int size() const { return size_; }
	inline int length() const { return size_; }
	
	bool lower(const AbstractArray& b) const
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
	
	bool equal(const AbstractArray& b) const
	{
		bool value = (size_ == b.size_);
		for (int i = 0; value && (i < size_); ++i)
			value = (buf_[i] == b.buf_[i]);
		return value;
	}

protected:
	int size_;
	T* buf_;
};

template<class T>
class Array: public AbstractArray<T>
{
public:
	typedef T Element;
	typedef AbstractArray<T> Super;
	
	Array()
	{}
	
	Array(int size)
		: AbstractArray<T>((size > 0) ? new T[size] : 0, size)
	{
		if (PONA_IS_ATOMIC(T))
			clear();
	}
	
	template<class T2>
	Array(T2* buf)
	{
		if (buf) {
			Super::size_ = pona::strlen(buf);
			Super::buf_ = new T[Super::size_];
			for (int i = 0; i < Super::size_; ++i)
				Super::buf_[i] = buf[i];
		}
	}
	
	template<class T2>
	Array(T2* buf, int size)
		: AbstractArray<T>((size > 0) ? new T[size] : 0, size)
	{
		for (int i = 0; i < size; ++i)
			Super::buf_[i] = buf[i];
	}
	
	~Array()
	{
		if (Super::buf_) {
			delete[] Super::buf_;
			Super::buf_ = 0;
		}
	}
	
	inline Array& operator=(const Array& b)
	{
		if (Super::size_ != b.size_) {
			if (Super::buf_)
				delete[] Super::buf_;
			Super::size_ = b.size_;
			Super::buf_ = (b.size_ > 0) ? new T[b.size_] : 0;
		}
		
		for (int i = 0; i < Super::size_; ++i)
			Super::buf_[i] = b.buf_[i];
			
		return *this;
	}
	
	Array(const Array& b) { *this = b; }
	
	template<class T1, class T2>
	Array(const Array<T1>& a, const Array<T2>& b)
	{
		Super::size_ = a.size() + b.size();
		Super::buf_ = (Super::size_ > 0) ? new T[Super::size_] : 0;
		int i = 0, an = a.size();
		while (i < an) {
			Super::buf_[i] = a.buf_[i];
			++i;
		}
		int j = 0, bn = b.size();
		while (j < bn) {
			Super::buf_[i] = b.buf_[j];
			++i;
			++j;
		}
	}
	
	inline void clear(const T& value = T())
	{
		for (int i = 0; i < Super::size_; ++i)
			Super::buf_[i] = value;
	}
	
	inline void set(int i, T e)
	{
		assert(Super::def(i));
		Super::buf_[i] = e;
	}
	
	inline bool operator<(const Array& b) const { return lower(b); }
	inline bool operator==(const Array& b) const { return equal(b); }
	inline bool operator>(const Array& b) const { return b < *this; }
	inline bool operator!=(const Array& b) const { return !(*this == b); }
	inline bool operator<=(const Array& b) const { return (*this < b) || (*this == b); }
	inline bool operator>=(const Array& b) const { return (b < *this) || (*this == b); }
	
	inline T* pointerAt(int i) const
	{
		assert(Super::def(i));
		return Super::buf_ + i;
	}
	
	inline T& at(int i) const
	{
		assert(Super::def(i));
		return Super::buf_[i];
	}
	inline T* data() const { return Super::buf_; }
	inline operator T*() const { return Super::buf_; }
	inline operator bool() const { return Super::buf_; }
};

template<class T>
class Array<const T>: public AbstractArray<T>
{
public:
	typedef T Element;
	typedef AbstractArray<T> Super;
	
	Array()
	{}
	
	Array(const T* buf)
		: AbstractArray<T>(const_cast<T*>(buf), (buf) ? pona::strlen(buf) : 0)
	{}
	
	Array(const T* buf, int size)
		: AbstractArray<T>(const_cast<T*>(buf), size)
	{}
	
	inline bool operator<(const Array& b) const { return lower(b); }
	inline bool operator==(const Array& b) const { return equal(b); }
	inline bool operator>(const Array& b) const { return b < *this; }
	inline bool operator!=(const Array& b) const { return !(*this == b); }
	inline bool operator<=(const Array& b) const { return (*this < b) || (*this == b); }
	inline bool operator>=(const Array& b) const { return (b < *this) || (*this == b); }
	
	inline const T* pointerAt(int i) const
	{
		assert(Super::def(i));
		return Super::buf_ + i;
	}
	
	inline const T& at(int i) const
	{
		assert(Super::def(i));
		return Super::buf_[i];
	}
	
	inline const T* data() const { return Super::buf_; }
	inline operator bool() const { return Super::buf_; }
	inline operator const T*() const { return Super::buf_; }
};

} // namespace pona

#endif // PONA_ARRAY_HPP
