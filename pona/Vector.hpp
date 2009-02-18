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

#include "Atoms.hpp"

namespace pona
{

template<class T, int defaultSize = 128>
class Vector: public Instance
{
public:
	typedef T Element;
	
	Vector(T* buf, int size = defaultSize)
		: bufOwner_(false),
		  buf_(buf),
		  bufSize_(size)
	{}
	
	Vector(int size = defaultSize)
		: bufOwner_(true),
		  buf_(new T[size]),
		  bufSize_(size)
	{}
	
	~Vector()
	{
		if (bufOwner_)
		{
			delete[] buf_;
			buf_ = 0;
		}
	}
	
	/*inline operator const T*() const { return buf_; }
	inline operator T*() { return buf_; }*/
	
	inline void clear(const T& value = T())
	{
		for (int i = 0; i < bufSize_; ++i)
			buf_[i] = value;
	}
	
	inline int size() const { return bufSize_; }
	
	inline int length() const { return bufSize_; }
	inline void set(int i, T e) { buf_[i] = e; }
	inline T get(int i) { return buf_[i]; }
	
private:
	Vector(const Vector& b);
	const Vector& operator=(const Vector& b);

	bool bufOwner_;
	T* buf_;
	int bufSize_;
};

} // namespace pona

#endif // PONA_VECTOR_HPP

