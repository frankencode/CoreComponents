/*
 * List.hpp -- chunked double-linked list with cached random access
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_LIST_HPP
#define PONA_LIST_HPP

#include <new>
#include "atoms"
#include "Sequence.hpp"
#include "ListObserver.hpp"

namespace pona
{

PONA_EXCEPTION(ListException, Exception);

template<class T>
class List: public Sequence<T>, public NonCopyable
{
public:
	List();
	~List();
	
	inline void pushBack(const T& e) { push(length_, 1, &e); }
	inline T popBack() { T e; pop(length_-1, 1, &e); return e; }
	inline void pushFront(const T& e) { push(0, 1, &e); }
	inline T popFront() { T e; pop(0, 1, &e); return e; }
	inline T front(int i) const { return get(i); }
	inline T back(int i) const { return get(length_-(i + 1)); }
	inline T first() const { return get(0); }
	inline T last() const { return get(-1); }
	
	inline void append(const T& e) { push(length_, 1, &e); }
	inline void insert(int i, const T& e) { push(i, 1, &e); }
	inline void remove(int i, T* e = 0) { pop(i, 1, e); }
	inline void remove(int i, int n) { pop(i, n); }
	inline int length() const { return length_; }
	
	typedef ListObserver<T> Observer;
	inline void setObserver(Ref<Observer> observer) { observer_ = observer; }
	
	void push(int i, int n = 1, const T* v = 0);
	void pop(int i, int n = 1, T* v = 0);
	
	void read(int i, int n, T* v) const;
	void write(int i, int n, const T* v);
	void fill(int i, int n,  const T& e);
	
	void clear();
	
	inline bool def(int i) const { return ((0 <= i) && (i < length_)) || ((-length_ <= i) && (i < 0)); }
	T get(int i) const;
	void set(int i, T e);
	
	inline Ref<List, Owner> range(int i, int n) const { return newChildList(this, i, n); }
	inline Ref<List, Owner> copy(int i, int n) const;
	inline Ref<List, Owner> copy() const { return copy(0, length_); }
	
	inline int find(const T& e) const { return find(0, length_, e); }
	inline int find(int i, const T& e) const { return find(i, length_ - i, e); }
	inline bool contains(const T& e) const { return find(e) != length_; }
	int replace(const T& a, const T& b);
	
	inline void insert(int i, List* b) { push(i, b->length_, b); }
	inline void append(List* b) { insert(length_, b); }
	inline int find(List* b) const { return find(0, length_, b); }
	inline int find(int i, List* b) const { return find(i, length_ - i, b); }
	inline bool contains(List* b) const { return find(0, length_, b) != length_; }
	
	bool operator<(const List& b) const;
	bool operator==(const List& b) const;
	inline bool operator>(const List& b) const { return b < *this; }
	inline bool operator!=(const List& b) const { return !(*this == b); }
	inline bool operator<=(const List& b) const { return (*this < b) || (*this == b); }
	inline bool operator>=(const List& b) const { return (b < *this) || (*this == b); }
	
	/*
	void sort();
	void unique(Ref< List<int> > frq = 0);
	void statistic(Ref< List<T> > members, Ref< List<int> > frq);
	*/
	
	void enableIndexCaching() { if (!posCached_) posCached_ = new Pos; }
	
protected:
	List(Ref<List> parent, int index0, int length);
	virtual Ref<List, Owner> newList() const { return new List; }
	virtual Ref<List, Owner> newChildList(Ref<List> parent, int i, int n) const { return new List(parent, i, n); }
	
private:
	int find(int i, int n, const T& e) const;
	void push(int i, int n, List* b);
	int find(int i, int n, List* b) const;
	
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
		inline const T& v(int k) const { return reinterpret_cast<const T*>(this+1)[k]; }
		
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
		
		inline const T& get() const { return node_->v(k_); }
		inline void set(T e) { *(node_->v() + k_) = e; }
	};
	
	Pos translate(int i) const
	{
		Pos pos;
		if (posCached_) {
			pos = *posCached_;
			int d0 = pos.distance(i);
			
			if (d0 > 5) {
				Pos posFront = Pos(front_, 0, 0);
				Pos posBack = Pos(back_, length_-1, back_->n_-1);
				
				int d1 = posFront.distance(i);
				int d2 = posBack.distance(i);
				if ((d1 < d0) && (d1 < d2))
					pos = posFront;
				else if (d2 < d0)
					pos = posBack;
			}
		
			pos.stepTo(i);
			*posCached_ = pos;
		}
		else {
			pos = (i < length_ / 2) ? Pos(front_, 0, 0) : Pos(back_, length_-1, back_->n_-1);
			pos.stepTo(i);
		}
		return pos;
	}
	
	Ref< List<T>, Owner > parent_;
	int index0_;
	int length_;
	
	Node* front_;
	Node* back_;
	mutable Pos* posCached_;
	
	Ref<Observer, Owner> observer_;
};

template<class T>
List<T>::List()
	: length_(0),
	  front_(0),
	  back_(0),
	  posCached_(0)
{}

template<class T>
List<T>::List(Ref<List> parent, int index0, int length)
	: parent_(parent),
	  index0_(index0),
	  length_(length),
	  front_(0),
	  back_(0),
	  posCached_(0)
{}

template<class T>
List<T>::~List()
{
	if (!parent_)
		clear();
	if (posCached_) {
		delete posCached_;
		posCached_ = 0;
	}
}

template<class T>
void List<T>::push(int i, int n, const T* v)
{
	if (n == 0) return;
	
	if (i < 0) i += length_ + 1;
	if (!((0 <= i) && (i <= length_)))
		PONA_THROW(ListException, "Wrong index");
	
	if (observer_)
		if (!observer_->beforePush(i, n, v))
			return;
	
	if (parent_)
	{
		parent_->push(i + index0_, n, v);
	}
	else
	{
		if (length_ == 0)
		{
			front_ = back_ = createNode(0, 0, v, n);
		}
		else
		{
			if (i == 0)
			{
				front_ = createNode(0, front_, v, n);
				if (posCached_) *posCached_ = Pos();
			}
			else if (i == length_)
			{
				back_ = createNode(back_, 0, v, n);
				if (posCached_) *posCached_ = Pos();
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
				
				if (posCached_) *posCached_ = Pos(node, i, 0);
			}
		}
	}
	
	length_ += n;
	
	if (observer_) observer_->afterPush(i, n);
}

template<class T>
void List<T>::pop(int i, int n, T* v)
{
	if (n == 0) return;
	
	if (i < 0) i += length_;
	if (!((0 <= i) && (i + n <= length_)))
		PONA_THROW(ListException, "Wrong index");
	
	if (observer_)
		if (!observer_->beforePop(i, n))
			return;
	
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
				
			if (posCached_) {
				if (node0->pred_ != 0)
					*posCached_ = Pos(node0->pred_, i-1, node0->pred_->n_-1);
				else if (node1->succ_ != 0)
					*posCached_ = Pos(node1->succ_, i, 0);
				else
					*posCached_ = Pos();
			}
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
			
			if (posCached_)
				*posCached_ = Pos(node, i, k0);
		}
		
		node1->succ_ = 0;
		while (node0) {
			node1 = node0->succ_;
			destroyNode(node0);
			node0 = node1;
		}
	}
	
	length_ -= n;
	
	if (observer_) observer_->afterPop(i, n);
}

template<class T>
void List<T>::read(int i, int n, T* v) const
{
	if (n == 0) return;
	
	if (i < 0) i += length_;
	if (!((0 <= i) && (i + n <= length_)))
		PONA_THROW(ListException, "Wrong index");
	
	if (parent_) {
		parent_->read(i + index0_, n, v);
	}
	else {
		Pos pos = translate(i);
		for (int i = 0; i < n; ++i) {
			v[i] = pos.get();
			pos.step();
		}
	}
}

template<class T>
void List<T>::write(int i, int n, const T* v)
{
	if (n == 0) return;
	
	if (i < 0) i += length_;
	if (!((0 <= i) && (i + n <= length_)))
		PONA_THROW(ListException, "Wrong index");
	
	if (observer_)
		if (!observer_->beforeWrite(i, n, v))
			return;
	
	if (parent_) {
		parent_->write(i + index0_, n, v);
	}
	else {
		Pos pos = translate(i);
		for (int i = 0; i < n; ++i) {
			pos.set(v[i]);
			pos.step();
		}
	}
	
	if (observer_) observer_->afterWrite(i, n);
}

template<class T>
void List<T>::fill(int i, int n, const T& e)
{
	Array<T> v(n);
	v.clear(e);
	write(i, n, v.data());
}

template<class T>
void List<T>::clear()
{
	int n = length_;
	if (n == 0) return;
	
	if (observer_) 
		if (!observer_->beforePop(0, n))
			return;
	
	if (parent_) {
		parent_->pop(index0_, length_);
	}
	else {
		Node* node = front_;
		while (node) {
			Node* succ = node->succ_;
			destroyNode(node);
			node = succ;
		}
		front_ = back_ = 0;
		if (posCached_)
			*posCached_ = Pos();
		length_ = 0;
		
	}
	
	if (observer_) observer_->afterPop(0, n);
}

template<class T>
inline T List<T>::get(int i) const
{
	if (i < 0) i += length_;
	if (!((0 <= i) && (i < length_)))
		PONA_THROW(ListException, "Wrong index");
	
	T e = (parent_) ? parent_->get(i + index0_) : translate(i).get();
	return e;
}

template<class T>
inline void List<T>::set(int i, T e)
{
	if (i < 0) i += length_;
	if (!((0 <= i) && (i < length_)))
		PONA_THROW(ListException, "Wrong index");
	
	if (observer_)
		if (!observer_->beforeWrite(i, 1, &e))
			return;
	
	if (parent_)
		parent_->set(i + index0_, e);
	else
		translate(i).set(e);
	
	if (observer_) observer_->afterWrite(i, 1);
}

template<class T>
Ref<List<T>, Owner> List<T>::copy(int i, int n) const
{
	if (i < 0) i += length_;
	if (!((0 <= i) && (i + n <= length_)))
		PONA_THROW(ListException, "Wrong index or length");
	
	Ref<List, Owner> b;
	
	if (parent_) {
		b = parent_->copy(i + index0_, n);
	}
	else {
		b = newList();
		
		if (n > 0) {
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
	
	return b;
}

template<class T>
int List<T>::find(int i, int n, const T& e) const
{
	if (length_ == 0) return 0;
	
	if (i < 0) i += length_;
	if (!((0 <= i) && (i < length_)))
		PONA_THROW(ListException, "Wrong index");
	
	if (parent_) {
		i = parent_->find(index0_ + i, n, e) - index0_;
	}
	else {
		Pos pos = translate(i);
		while (pos.i_ < i + n) {
			if (pos.get() == e) break;
			pos.step();
		}
		
		i = pos.i_;
	}
	
	return i;
}

template<class T>
int List<T>::replace(const T& a, const T& b)
{
	int i = 0;
	int n = 0;
	while (i < length_) {
		i = find(i, a);
		if (i == length_)
			break;
		set(i, b);
		++n;
		++i;
	}
	return n;
}

template<class T>
void List<T>::push(int i, int n, List* b)
{
	if (n == 0) return;
	
	if (i < 0) i += length_;
	if (!((0 <= i) && (i <= length_) && (n <= b->length_)))
		PONA_THROW(ListException, "Wrong index or length");
	
	if (observer_) {
		Array<T> v(b->length());
		b->read(0, b->length(), v.data());
		if (!observer_->beforePush(i, n, v))
			return;
	}
	
	if (parent_) {
		parent_->push(index0_ + i, n, b);
	}
	else {
		push(i, n);
		
		int k = 0;
		while (b->parent_) {
			k += b->index0_;
			b = b->parent_;
		}
		
		if (k + n > b->length_)
			PONA_THROW(ListException, "Wrong index or length");
		
		Pos pa = translate(i);
		Pos pb = b->translate(k);
		while (n > 0) {
			pa.set(pb.get());
			pa.step();
			pb.step();
			--n;
		}
	}
	
	if (observer_) observer_->afterPush(i, n);
}

template<class T>
int List<T>::find(int i, int n, List* b) const
{
	if (length_ == 0) return 0;
	
	if (i < 0) i += length_;
	if (!((0 <= i) && (i < length_)))
		PONA_THROW(ListException, "Wrong index");
	
	if (n == 0) return 0;
	if (b->length_ == 0) return i;
	
	if (parent_) {
		i = parent_->find(index0_ + i, n, b) - index0_;
	}
	else {
		int k = 0, m = b->length_, m0 = m;
		while (b->parent_) {
			k += b->index0_;
			b = b->parent_;
		}
		
		Pos pa = translate(i);
		Pos pb = b->translate(k), pb0 = pb;
		while ((n > 0) && (m > 0)) {
			if (pa.get() == pb.get()) {
				pb.step();
				--m;
			}
			else {
				pb = pb0;
				m = m0;
			}
			pa.step();
			--n;
		}
		
		i = pa.i_;
		if (m == 0) i -= m0;
	}
	
	return i;
}

template<class T>
bool List<T>::operator<(const List& b_) const
{
	const List* a = this;
	int i = 0, n = length_;
	while (a->parent_) {
		i += a->index0_;
		a = a->parent_;
	}
	
	const List* b = &b_;
	int k = 0, m = b->length_;
	while (b->parent_) {
		k += b->index0_;
		b = b->parent_;
	}
	
	if ((n > 0) && (m > 0)) {
		Pos pa = a->translate(i);
		Pos pb = b->translate(k);
		while ((n > 0) && (m > 0)) {
			T ca = pa.get();
			T cb = pb.get();
			if (ca < cb)
				return true;
			else if (cb < ca)
				return false;
			pa.step();
			pb.step();
			--n;
			--m;
		}
	}
	
	return n < m;
}

template<class T>
bool List<T>::operator==(const List& b_) const
{
	const List* a = this;
	int i = 0, n = length_;
	while (a->parent_) {
		i += a->index0_;
		a = a->parent_;
	}
	
	const List* b = &b_;
	int k = 0, m = b->length_;
	while (b->parent_) {
		k += b->index0_;
		b = b->parent_;
	}
	
	bool equal = (n == m);
	
	if (equal && (n > 0)) {
		Pos pa = a->translate(i);
		Pos pb = b->translate(k);
		while (equal && (n > 0)) {
			equal = (pa.get() == pb.get());
			pa.step();
			pb.step();
			--n;
		}
	}
	
	return equal;
}

} // namespace pona

#endif // PONA_LIST_HPP
