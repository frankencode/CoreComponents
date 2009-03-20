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

#ifndef PONA_LIST_HPP
#define PONA_LIST_HPP

#include <new>
#include "atoms"
#include "Monitor.hpp"
#include "RandomAccessMedia.hpp"

namespace pona
{

PONA_EXCEPTION(ListException, Exception);

/** \brief Dynamic double-linked list
  *
  * provides:
  *  - delivers length() in constant time
  *  - medium overhead in memory consumption
  *  - provides a highlevel index based interface and thereby omits the risk of
  *    dangling pointers or invalid iteration objects
  *  - most index based access schemes are supported in constant time utilizing
  *    an index cache
  *  - allows non-overlapping child lists of possibly more than one cascading level
  *  - throws exception on invalid operations
  *  - allows to limit the maximum number of nodes
  *
  * limitations:
  *  - on non-sequential random access or concurrent sequential access
  *    the index-based access may degrate to linear time
  *  - is not thread-safe by itself
  */
template<class T>
class List: public MonitorMedia, public RandomAccessMedia<T>
{
public:
	List(int maxLength = intMax);
	~List();
	
	inline void pushBack(T e) { push(length_, 1, &e); }
	inline T popBack() { T e; pop(length_-1, 1, &e); return e; }
	inline void pushFront(T e) { push(0, 1, &e); }
	inline T popFront() { T e; pop(0, 1, &e); return e; }
	inline T front(int i) { return get(i); }
	inline T back(int i) { return get(-(i + 1)); }
	
	inline void append(T e) { push(length_, 1, &e); }
	inline void insert(int i, T e) { push(i, 1, &e); }
	inline void remove(int i, T* e = 0) { pop(i, 1, e); }
	inline void remove(int i, int n) { pop(i, n); }
	inline int length() const { return length_; }
	inline int maxLength() const { return maxLength_; }
	
	inline void insert(int i, List* b) {
		int n = b->length();
		if (n > 0) {
			push(i, n);
			for (int k = 0; k < n; ++k)
				set(i+k, b->get(k));  // quick HACK, low performance
		}
	}
	inline void append(List* b) { insert(length_, b); }
	
	inline int size() const { return maxLength_; }
	inline int fill() const { return length_; }
	inline void resize(int n) { maxLength_ = n; }
	inline void setMonitor(Ref<Monitor> monitor) { monitor_ = monitor; }
	
	void push(int i, int n = 1, const T* v = 0);
	void pop(int i, int n = 1, T* v = 0);
	
	void read(int i, int n, T* v);
	void write(int i, int n, T* v);
	
	void clear();
	
	T get(int i) const;
	void set(int i, T e);
	
	inline Ref<List, Owner> range(int i, int n) { return new List(this, i, n); }
	inline Ref<List, Owner> copy(int i, int n);
	inline Ref<List, Owner> copy() { return copy(0, length_); }
	
	/*
	void sort();
	void unique(Ref< List<int> > frq = 0);
	void statistic(Ref< List<T> > members, Ref< List<int> > frq);
	*/
	
	bool operator<(const List& b) const;
	bool operator==(const List& b) const;
	inline bool operator>(const List& b) const { return b < *this; }
	inline bool operator!=(const List& b) const { return !(*this == b); }
	inline bool operator<=(const List& b) const { return (*this < b) || (*this == b); }
	inline bool operator>=(const List& b) const { return (b < *this) || (*this == b); }
	
protected:
	List(Ref<List> parent, int index0, int length);
	
private:
	PONA_DISABLE_COPY(List)
	
	class Node
	{
	public:
		Node(Node* pred, Node* succ, const T* v, int n)
		{
			pred_ = pred;
			succ_ = succ;
			n_ = n;
			
			T* vd = Node::v();
			
			if (!PONA_IS_ATOMIC(T))
				for (int i = 0; i < n; ++i)
					new(vd+i)T();
			
			if (v != 0) {
				for (int i = 0; i < n; ++i)
					vd[i] = v[i];
			}
			
			if (pred_) pred_->succ_ = this;
			if (succ_) succ_->pred_ = this;
		}
		
		~Node() {}
		
		inline T* v() { return reinterpret_cast<T*>(this+1); }
		inline T v(int k) const { return reinterpret_cast<const T*>(this+1)[k]; }
		
		Node* pred_;
		Node* succ_;
		int n_;
	};
	
	inline Node* createNode(Node* pred, Node* succ, const T* v, int n)
	{
		int nodeSize = sizeof(Node) + sizeof(T) * n;
		return new(new uint8_t[nodeSize]) Node(pred, succ, v, n);
	}
	
	inline void destroyNode(Node* node)
	{
		if (!PONA_IS_ATOMIC(T))
		{
			T* vd = node->v();
			int i = node->n_;
			while (i--)
				vd[i].~T();
		}
		node->~Node();
		delete[] reinterpret_cast<uint8_t*>(node);
	}
		
	class Pos
	{
	public:
		Pos()
			: node_(0),
			  i_(0),
			  k_(0)
		{}
		
		Pos(Node* node, int i, int k)
			: node_(node),
			  i_(i),
			  k_(k)
		{}
		
		Node* node_;
		int i_, k_;
		
		inline bool operator==(const Pos& b) const { return i_ == b.i_; }
		inline bool operator<(const Pos& b) const { return node_->v(k_) < b.node_->v(b.k_); }
		
		inline operator bool() const { return node_ != 0; }
		inline int distance(int i) const { return (node_ == 0) ? intMax : ((i > i_) ? i - i_ : i_ - i); }
		
		void stepTo(int i)
		{
			// step forward (towards back)
			while (i_ < i)
			{
				int i0Succ = i_ - k_ + node_->n_;
				
				if (i < i0Succ) {
					k_ += i - i_;
					i_ = i;
				}
				else {
					node_ = node_->succ_;
					i_ = i0Succ;
					k_ = 0;
				}
			}
			
			// step backward (towards front)
			while (i_ > i)
			{
				int i1Pred = i_ - k_ - 1;
				
				if (i > i1Pred) {
					k_ -= i_ - i;
					i_ = i;
				}
				else {
					node_ = node_->pred_;
					i_ = i1Pred;
					k_ = node_->n_-1;
				}
			}
		}
		
		inline void step()
		{
			++i_;
			++k_;
			if (k_ == node_->n_) {
				k_ = 0;
				node_ = node_->succ_;
			}
		}
		
		inline T get() const { return node_->v(k_); }
		inline void set(T e) { *(node_->v() + k_) = e; }
	};
	
	inline Pos translate(int i) const
	{
		Pos posFront = Pos(front_, 0, 0);
		Pos posBack = Pos(back_, length_-1, back_->n_-1);
		
		int d0 = posCached_.distance(i);
		int d1 = posFront.distance(i);
		int d2 = posBack.distance(i);
		
		Pos pos = posCached_;
		
		if ((d1 < d0) && (d1 < d2))
			pos = posFront;
		else if (d2 < d0)
			pos = posBack;
		
		pos.stepTo(i);
		posCached_ = pos;
		
		return pos;
	}
	
	Ref< List<T>, Owner > parent_;
	int index0_;
	
	int maxLength_;
	int length_;
	
	Node* front_;
	Node* back_;
	mutable Pos posCached_;
	
	Ref<Monitor, Owner> monitor_;
};

template<class T>
List<T>::List(int maxLength)
	: maxLength_(maxLength),
	  length_(0),
	  front_(0),
	  back_(0)
{}

template<class T>
List<T>::List(Ref<List> parent, int index0, int length)
	: parent_(parent),
	  index0_(index0),
	  maxLength_(intMax),
	  length_(length),
	  front_(0),
	  back_(0)
{}

template<class T>
List<T>::~List()
{
	if (!parent_)
		clear();
}

template<class T>
void List<T>::push(int i, int n, const T* v)
{
	if (n == 0) return;
	
	if (i < 0) i += length_ + 1;
	if (monitor_) monitor_->beforeGrowing(i, n);
	
	if (!((0 <= i) && (i <= length_)))
	{
		if (monitor_) monitor_->afterGrowing(i, n);
		PONA_THROW(ListException, "Wrong index");
	}
	
	if (parent_)
	{
		parent_->push(i + index0_, n, v);
	}
	else
	{
		if (length_ + n > maxLength_)
		{
			if (monitor_) monitor_->afterGrowing(i, n);
			PONA_THROW(ListException, "List overflow");
		}
		
		if (length_ == 0)
		{
			front_ = back_ = createNode(0, 0, v, n);
		}
		else
		{
			if (i == 0)
			{
				front_ = createNode(0, front_, v, n);
				posCached_ = Pos();
			}
			else if (i == length_)
			{
				back_ = createNode(back_, 0, v, n);
				posCached_ = Pos();
			}
			else
			{
				Pos pos = translate(i);
				
				Node* node = pos.node_;
				int k = pos.k_;
				
				bool isFront = (node == front_);
				bool isBack = (node == back_);
				
				Node* pred;
				Node* succ;
				
				if (k == 0)
				{
					pred = node->pred_;
					succ = node;
				}
				else
				{
					// split node: [0..n-1] => [0..k-1],[k..n-1]
					pred = createNode(node->pred_, node->succ_, node->v(), k);
					succ = createNode(pred, node->succ_, node->v() + k, node->n_ - k);
					destroyNode(node);
				}
				
				node = createNode(pred, succ, v, n);
				
				if (isFront) front_ = pred;
				if (isBack) back_ = succ;
				
				posCached_ = Pos(node, i, 0);
			}
		}
	}
	
	length_ += n;
	
	if (monitor_) monitor_->afterGrowing(i, n);
}

template<class T>
void List<T>::pop(int i, int n, T* v)
{
	if (n == 0) return;
	
	if (i < 0) i += length_;
	if (monitor_) monitor_->beforeShrinking(i, n);
	
	if (!((0 <= i) && (i + n <= length_)))
	{
		if (monitor_) monitor_->afterShrinking(i, n);
		PONA_THROW(ListException, "Wrong index");
	}
	
	if (parent_)
	{
		parent_->pop(i + index0_, n, v);
	}
	else
	{
		Pos pos0 = translate(i);
		Pos pos1 = translate(i + n - 1);
		
		if (v != 0)
		{
			Pos pos = pos0;
			for (int i = 0; i < n; ++i)
			{
				v[i] = pos.get();
				pos.step();
			}
		}
		
		Node* node0 = pos0.node_;
		Node* node1 = pos1.node_;
		int k0 = pos0.k_;
		int k1 = pos1.k_;
		
		if ((k0 == 0) && (k1 == node1->n_ - 1))
		{
			if (node0->pred_ != 0)
				node0->pred_->succ_ = node1->succ_;
			if (node1->succ_ != 0)
				node1->succ_->pred_ = node0->pred_;
			
			if (node0 == front_)
				front_ = node1->succ_ ;
			if (node1 == back_)
				back_ = node0->pred_;
				
			if (node0->pred_ != 0)
				posCached_ = Pos(node0->pred_, i-1, node0->pred_->n_-1);
			else if (node1->succ_ != 0)
				posCached_ = Pos(node1->succ_, i, 0);
			else
				posCached_ = Pos();
		}
		else
		{
			// merge nodes: node0([0..k0-1]), node1([k1+1..n1-1]) => node
			Node* node = createNode(node0->pred_, node1->succ_, 0, k0 + node1->n_ - k1 - 1);
			T* vd = node->v(); // dest
			int kd = 0;
			{
				T* vs = node0->v(); // src
				for (int ks = 0; ks < k0; ++ks, ++kd)
					vd[kd] = vs[ks];
			}
			{
				T* vs = node1->v(); // src
				for (int ks = k1 + 1, n1 = node1->n_; ks < n1; ++ks, ++kd)
					vd[kd] = vs[ks];
			}
			if (node0 == front_)
				front_ = node;
			if (node0 == back_)
				back_ = node;
			if (node1 == back_)
				back_ = node;
			
			posCached_ = Pos(node, i, k0);
		}
		
		node1->succ_ = 0;
		while (node0) {
			node1 = node0->succ_;
			destroyNode(node0);
			node0 = node1;
		}
	}
	
	length_ -= n;
	
	if (monitor_) monitor_->afterShrinking(i, n);
}

template<class T>
void List<T>::write(int i, int n, T* v)
{
	if (n == 0) return;
	
	if (i < 0) i += length_;
	if (monitor_) monitor_->beforeAccess(i, n);
	
	if (!((0 <= i) && (i + n <= length_)))
	{
		if (monitor_) monitor_->afterAccess(i, n);
		PONA_THROW(ListException, "Wrong index");
	}
	
	if (parent_)
	{
		parent_->write(i + index0_, n, v);
	}
	else
	{
		Pos pos = translate(i);
		for (int i = 0; i < n; ++i)
		{
			pos.set(v[i]);
			pos.step();
		}
	}
	
	if (monitor_) monitor_->afterAccess(i, n);
}

template<class T>
void List<T>::read(int i, int n, T* v)
{
	if (n == 0) return;
	
	if (i < 0) i += length_;
	if (monitor_) monitor_->beforeReadonlyAccess(i, n);
	
	if (!((0 <= i) && (i + n <= length_)))
	{
		if (monitor_) monitor_->afterReadonlyAccess(i, n);
		PONA_THROW(ListException, "Wrong index");
	}
	
	if (parent_)
	{
		parent_->read(i + index0_, n, v);
	}
	else
	{
		Pos pos = translate(i);
		for (int i = 0; i < n; ++i)
		{
			v[i] = pos.get();
			pos.step();
		}
	}
	
	if (monitor_) monitor_->afterReadonlyAccess(i, n);
}

template<class T>
void List<T>::clear()
{
	int n = length_;
	if (n == 0) return;
	
	if (monitor_) monitor_->beforeShrinking(0, n);
	
	if (parent_)
	{
		parent_->pop(index0_, length_);
	}
	else
	{
		Node* node = front_;
		while (node)
		{
			Node* succ = node->succ_;
			destroyNode(node);
			node = succ;
		}
		front_ = back_ = 0;
		posCached_ = Pos();
		length_ = 0;
		
	}
	
	if (monitor_) monitor_->afterShrinking(0, n);
}

template<class T>
inline T List<T>::get(int i) const
{
	if (i < 0) i += length_;
	if (monitor_) monitor_->beforeReadonlyAccess(i, 1);
	
	if (!((0 <= i) && (i < length_)))
	{
		if (monitor_) monitor_->afterReadonlyAccess(i, 1);
		PONA_THROW(ListException, "Wrong index");
	}
	
	T e = (parent_) ? parent_->get(i + index0_) : translate(i).get();
	if (monitor_) monitor_->afterReadonlyAccess(i, 1);
	return e;
}

template<class T>
inline void List<T>::set(int i, T e)
{
	if (i < 0) i += length_;
	if (monitor_) monitor_->beforeAccess(i, 1);
	
	if (!((0 <= i) && (i < length_)))
	{
		if (monitor_) monitor_->afterAccess(i, 1);
		PONA_THROW(ListException, "Wrong index");
	}
		
	if (parent_)
		parent_->set(i + index0_, e);
	else
		translate(i).set(e);
	
	if (monitor_) monitor_->afterAccess(i, 1);
}

template<class T>
Ref<List<T>, Owner> List<T>::copy(int i, int n)
{
	if (i < 0) i += length_;
	if (monitor_) monitor_->beforeReadonlyAccess(i, n);
	
	if (!((0 <= i) && (i + n <= length_)))
	{
		if (monitor_) monitor_->afterReadonlyAccess(i, n);
		PONA_THROW(ListException, "Wrong index or length");
	}
	
	Ref<List, Owner> b;
	
	if (parent_)
	{
		b = parent_->copy(i + index0_, n);
	}
	else
	{
		b = new List();
		
		if (n > 0)
		{
			b->push(0, n);
			Pos src = translate(i);
			Pos dst = b->translate(0);
			while (dst) {
				dst.set(src.get());
				src.step();
				dst.step();
			}
		}
	}
	
	if (monitor_) monitor_->afterReadonlyAccess(i, n);
	
	return b;
}

template<class T>
bool List<T>::operator<(const List& b) const
{
	int n = (length_ < b.length_) ? length_ : b.length_;
	int i = 0;
	while (i < n) {
		T c1 = get(i);
		T c2 = b.get(i);
		if ((c1 < c2) || (c2 < c1))
			return c1 < c2;
		++i;
	}
	return length_ < b.length_;
}

template<class T>
bool List<T>::operator==(const List& b) const
{
	bool equal = (length_ == b.length_);
	int i = 0;
	while (equal && (i < length_)) {
		T c1 = get(i);
		T c2 = b.get(i);
		equal = !((c1 < c2) || (c2 < c1));
		++i;
	}
	return equal;
}

} // namespace pona

#endif // PONA_LIST_HPP
