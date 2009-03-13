/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_VECTOR_HPP
#define PONA_VECTOR_HPP

#include "atoms"
#include "RandomAccessMedia.hpp"

namespace pona
{

template<class T>
class Vector: public Instance, public RandomAccessMedia<T>
{
public:
	typedef T Element;
	
	Vector()
		: size_(0),
		  buf_(0),
		  owner_(false)
	{}
	
	Vector(int size)
		: size_(size),
		  buf_((size > 0) ? new T[size] : 0),
		  owner_(true)
	{
		if (PONA_IS_ATOMIC(T))
			clear();
	}
	
	template<class T2>
	Vector(T2* buf)
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
	Vector(T2* buf, int size)
		: size_(size),
		  buf_((size > 0) ? new T[size] : 0),
		  owner_(size > 0)
	{
		for (int i = 0; i < size; ++i)
			buf_[i] = buf[i];
	}
	
	~Vector()
	{
		if (owner_) {
			if (buf_) {
				delete[] buf_;
				buf_ = 0;
			}
		}
	}
	
	Vector(const Vector& b)
	{
		*this = b;
	}
	
	inline const Vector& operator=(const Vector& b)
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
	
	inline bool operator<(const Vector& b) const
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
	
	inline bool operator==(const Vector& b) const
	{
		bool equal = (size_ == b.size_);
		for (int i = 0; equal && (i < size_); ++i)
			equal = (buf_[i] == b.buf_[i]);
		return equal;
	}
	
	inline bool operator>(const Vector& b) const { return b < *this; }
	inline bool operator!=(const Vector& b) const { return !(*this == b); }
	inline bool operator<=(const Vector& b) const { return (*this < b) || (*this == b); }
	inline bool operator>=(const Vector& b) const { return (b < *this) || (*this == b); }
	
	inline operator bool() const { return buf_; }
	
private:
	int size_;
	T* buf_;
	bool owner_;
};

} // namespace pona

#endif // PONA_VECTOR_HPP

