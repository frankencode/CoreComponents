/*
 * List.hpp -- double-linked list
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_LIST_HPP
#define FTL_LIST_HPP

#include "containers.hpp"
#include "Heap.hpp"
#include "ListNode.hpp"
#include "ListWalker.hpp"

namespace ftl
{

template<class T>
class List:
	public Container< T, List<T> >,
	public Sequence< T, ListWalker<T> >
{
public:
	typedef ListWalker<T> Index;
	typedef GenericIterator< List<T> > Iterator;
	typedef T Item;
	
	List(int maxLength = intMax);
	~List();
	
	inline Iterator iterator() const { return Iterator(this); }
	
	inline Return<Index> first() const { return Index(firstNode_, this); }
	inline Return<Index> last() const { return Index(lastNode_, this); }
	inline Return<Index> end() const { return Index(&endNode_, this); }
	
	inline bool isEmpty() const { return length_ == 0; }
	inline int length() const { return length_; }
	
	// item-vise list access
	inline bool def(Index index) const {
		check(index.list_ == this);
		return index.valid();
	}
	inline T get(Index index) const {
		return at(index);
	}
	inline const T& at(Index index) const {
		check(index.list_ == this);
		check(index.valid());
		return index.node_->item_;
	}
	inline void set(Index index, const T& item) {
		check(index.list_ == this);
		check(index.valid());
		index.node_->item_ = item;
	}
	
	// list structure manipulation
	List& push(Index& index, const T& item);
	List& pop(Index& index, T& item);
	void clear();
	
	inline T pop(Index& index) {
		Item item;
		pop(index, item);
		return item;
	}
	
	// generic container methods
	inline int size() const { return maxLength_; }
	inline int fill() const { return length_; }
	inline int isFull() const { return length_ >= maxLength_; }
	inline List& push(const T& item) { return push(end(), item); }
	inline List& pop(T& item) { return pop(first(), item); }
	inline T pop() { T item; pop(item); return item; }
	
	// random access helper
	inline bool def(int j) const {
		return (-length_ <= j) && (j < length_);
	}
	inline Return<Index> index(int j) const {
		if (j < 0) j += length_;
		if (def(j))
			return (j >= length_ / 2) ? last() - (length_ - j - 1) : first() + j;
		else
			return (j == length_) ? end() : Index(0, this);
	}
	inline T get(int j) const { return get(index(j)); }
	inline const T& at(int j) const { return at(index(j)); }
	inline void set(int j, const T& item) { set(index(j), item); }
	
	// convenience methods
	inline void append(const T& item) { push(end(), item); }
	inline void insert(Index& index, const T& item) { push(index, item); }
	inline void remove(Index& index, T& item) { pop(index, item); }
	inline void remove(Index& index) { pop(index); }
	inline void pushFront(const T& item) { push(first(), item); }
	inline void pushBack(const T& item) { push(end(), item); }
	inline T popFront() { return pop(first()); }
	inline T popBack() { return pop(last()); }
	
	// n-ary methods
	Index find(const T& item, Index index) const;
	inline Index find(const T& item) const { return find(item, first()); }
	int replace(const T& oldItem, const T& newItem);
	inline bool contains(const T& item) const { return def(find(item)); }
	inline T join(const T& sep = T()) const { return T::join(this, sep); }
	Ref<List, Owner> sort(int order = SortOrder::Ascending, bool unique = false) const;
	Ref<List, Owner> unique(int order = SortOrder::Ascending) const { return sort(order, true); }
	
	// return value to reference conversion wrappers
	inline List& push(Return<Index> ret, const T& item) {
		Index index = ret;
		return push(index, item);
	}
	inline List& pop(Return<Index> ret, T& item) {
		Index index = ret;
		return pop(index, item);
	}
	inline T pop(Return<Index> ret) {
		Index index = ret;
		Item item;
		pop(ret, item);
		return item;
	}
	inline List& insert(Return<Index> ret, const T& item) {
		Index index = ret;
		return push(index, item);
	}
	inline List& remove(Return<Index> ret, T& item) {
		Index index = ret;
		return pop(index, item);
	}
	
private:
	friend class ListWalker<T>;
	typedef ListNode<T> Node;
	
	Node endNode_;
	Node* firstNode_;
	Node* lastNode_;
	int length_;
	int maxLength_;
};

template<class T>
List<T>::List(int maxLength)
	: firstNode_(0),
	  lastNode_(0),
	  length_(0),
	  maxLength_(maxLength)
{}

template<class T>
List<T>::~List()
{
	clear();
}

template<class T>
List<T>& List<T>::push(Index& index, const T& item)
{
	check(index.list_ == this);
	check(index.valid() || index.atEnd() || isEmpty());
	
	Node* previousNode;
	Node* nextNode;
	if (index.atEnd()) {
		previousNode = lastNode_;
		nextNode = 0;
	}
	else {
		previousNode = (index.node_) ? index.node_->previous_ : 0;
		nextNode = index.node_;
	}
	Node* newNode = new Node(previousNode, nextNode, item);
	++length_;
	
	if (!previousNode)
		firstNode_ = newNode;
	if (!nextNode)
		lastNode_ = newNode;
	
	index.node_ = newNode;
	
	return *this;
}

template<class T>
List<T>& List<T>::pop(Index& index, T& oldItem)
{
	check(index.list_ == this);
	check(index.valid());
	
	Node* oldNode = index.node_;
	oldItem = oldNode->item_;
	
	if (oldNode->next_)
		index.node_ = oldNode->next_;
	else if (oldNode->previous_)
		index.node_ = oldNode->previous_;
	else
		index.node_ = 0;
	
	if (oldNode == firstNode_)
		firstNode_ = firstNode_->next_;
	if (oldNode == lastNode_)
		lastNode_ = lastNode_->previous_;
	
	delete oldNode;
	--length_;
	
	return *this;
}

template<class T>
void List<T>::clear()
{
	Node* node = lastNode_;
	while (node) {
		Node* oldNode = node;
		node = node->previous_;
		delete oldNode;
	}
	firstNode_ = 0;
	lastNode_ = 0;
	length_ = 0;
}

template<class T>
typename List<T>::Index List<T>::find(const T& item, Index index) const
{
	while (index.valid()) {
		if (at(index) == item) break;
		++index;
	}
	return index;
}

template<class T>
int List<T>::replace(const T& oldItem, const T& newItem)
{
	Index index = first();
	int numReplaced = 0;
	while (index.valid()) {
		if (at(index) == oldItem)
			set(index, newItem);
		++index;
		++numReplaced;
	}
	return numReplaced;
}

template<class T>
Ref<List<T>, Owner> List<T>::sort(int order, bool unique) const
{
	Ref<List, Owner> result = new List;
	if (length() == 0) return result;
	Heap<T> heap(length(), order);
	for (Index i = first(); def(i); ++i)
		heap.push(get(i));
	if (unique) {
		T prev = heap.next();
		result->append(prev);
		while (heap.hasNext()) {
			T item = heap.next();
			if (item != prev) {
				result->append(item);
				prev = item;
			}
		}
	}
	else {
		while (heap.hasNext())
			result->append(heap.next());
	}
	return result;
}

} // namespace ftl

#endif // FTL_LIST_H
