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

#ifndef PONA_HEAP_HPP
#define PONA_HEAP_HPP

#include "Atoms.hpp"

#ifdef PONA_WINDOWS
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#endif

namespace pona
{

template<class T>
class Heap: public Instance
{
public:
	enum SortOrder { Ascending = 0, Descending = 1 };
	
	Heap(int size, int order = Ascending)
		: fill_(0),
		  size_(size),
		  dscOrder_(order == Descending),
		  bufOwner_(true),
		  buf_(new T[size])
	{}
	
	Heap(T* buf, int size, int order = Ascending)
		: fill_(0),
		  size_(size),
		  dscOrder_(order == Descending),
		  bufOwner_(false),
		  buf_(buf)
	{}
	
	~Heap()
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

	inline void push(T e)
	{
		assert(fill_ < size_);
		buf_[fill_] = e;
		++fill_;
		passUpLast();
	}

	inline T pop()
	{
		assert(fill_ > 0);
		T e = buf_[0];
		--fill_;
		buf_[0] = buf_[fill_];
		passDownFromTop();
		return e;
	}

	inline void clear()
	{
		fill_ = 0;
	}
	
protected:
	PONA_DISABLE_COPY(Heap)
	
	int fill_;    // current number of elements
	int size_;    // maximal number of elements
	bool dscOrder_;    // used for ascending or descending sort?
	bool bufOwner_;
	T* buf_;    // memory buffer used for storing elements
	
	inline static int parent(int i) { return (i - 1) / 2; }
	inline static int leftChild(int i) { return 2*i + 1; }
	inline static int rightChild(int i) { return 2*i + 2; }

	void xchg(int i, int j);
	int min(int i, int j, int k);
	int max(int i, int j, int k);
	void passUpLast();
	void passDownFromTop();
};

template<class T>
inline void Heap<T>::xchg(int i, int j)
{
	T h = buf_[i];
	buf_[i] = buf_[j];
	buf_[j] = h;
}

template<class T>
inline int Heap<T>::min(int i, int j, int k)
{
	if ((buf_[i] < buf_[j]) && (buf_[i] < buf_[k])) return i;
	if ((buf_[j] < buf_[i]) && (buf_[j] < buf_[k])) return j;
	return k;
}

template<class T>
inline int Heap<T>::max(int i, int j, int k)
{
	if ((buf_[j] < buf_[i]) && (buf_[k] < buf_[i])) return i;
	if ((buf_[k] < buf_[j]) && (buf_[i] < buf_[j])) return j;
	return k;
}

template<class T>
void Heap<T>::passUpLast()
{
	if (fill_ == 1) return;
	
	int i = fill_ - 1;
	
	while (i != 0)
	{
		int j;
		j = parent(i);
		if ((buf_[j] < buf_[i]) == dscOrder_) break;
		xchg(i, j);
		i = j;
	}
}

template<class T>
void Heap<T>::passDownFromTop()
{
	if (fill_ == 0) return;
	
	int i = 0;
	
	while (true)
	{
		int j, lc, rc;
		lc = leftChild(i);
		rc = rightChild(i);
	
		if ((lc < fill_) && ((rc < fill_)))
		{
			j = (dscOrder_) ? min(i, lc, rc) : max(i, lc, rc);
			if (j == i) break;
			
			xchg(i, j);
			i = j;
			
			continue;
		}
		
		if (lc < fill_)
		{
			if ((buf_[i] < buf_[lc]) != dscOrder_) xchg(i, lc);
		}
		else
		if (rc < fill_)
		{
			if ((buf_[i] < buf_[rc]) != dscOrder_) xchg(i, rc);
		}
		
		break;
	}
}

} // namespace pona

#endif // PONA_HEAP_HPP
