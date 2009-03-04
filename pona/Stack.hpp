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

#ifndef PONA_STACK_HPP
#define PONA_STACK_HPP

#include "Atoms.hpp"

namespace pona
{

template<class T>
class Stack: public Instance
{
public:
	Stack(int size)
		: fill_(0),
		  size_(size),
		  bufOwner_(true),
		  buf_(new T[size])
	{}

	Stack(T* buf, int size)
		: fill_(0),
		  size_(size),
		  bufOwner_(false),
		  buf_(buf)
	{}
	
	~Stack()
	{
		if (bufOwner_)
		{
			delete[] buf_;
			buf_ = 0;
		}
	}

	inline int size() const { return size_; }
	inline int fill() const { return fill_; }

	inline bool isFull() const { return fill_ == size_; }
	inline bool isEmpty() const { return fill_ == 0; }

	inline void push(T x)
	{
		assert(fill_ < size_);
		buf_[fill_++] = x;
	}

	inline T pop()
	{
		assert(fill_ > 0);
		return buf_[--fill_];
	}

	inline void clear()
	{
		fill_ = 0;
	}

	inline T top(int i = 0) const
	{
		assert(i < fill_);
		return buf_[fill_-i-1];
	}

	inline T bottom(int i = 0) const
	{
		assert(i < fill_);
		return buf_[i];
	}

private:
	PONA_DISABLE_COPY(Stack)
	
	int fill_;
	int size_;
	bool bufOwner_;
	T* buf_;
};

} // namespace pona

#endif // PONA_STACK_HPP
