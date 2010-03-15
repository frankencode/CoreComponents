/*
 * CustomList.hpp -- double-linked list
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_NEWLIST_HPP
#define PONA_NEWLIST_HPP

#include "atoms"
#include "Sequence.hpp"
#include "ListNode.hpp"
#include "ListIterator.hpp"

namespace pona
{

template<class T>
class NewList: public Sequence< T, ListIterator<T> >, public NonCopyable
{
public:
	typedef ListIterator<T> Index;
	typedef T Item;
	
	NewList(int maxLength = intMax);
	~NewList();
	
	inline Return<Index> first() const { return Index(firstNode_, this); }
	inline Return<Index> last() const { return Index(lastNode_, this); }
	inline Return<Index> end() const { return Index(&endNode_, this); }
	
	inline bool empty() const { return length_ == 0; }
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
	NewList& push(Index& index, const T& item);
	NewList& pop(Index& index, T& item);
	inline T pop(Index& index) {
		Item item;
		pop(index, item);
		return item;
	}
	void clear();
	
	// generic container methods
	inline int size() const { return maxLength_; }
	inline int fill() const { return length_; }
	inline int full() const { return length_ >= maxLength_; }
	inline NewList& push(const T& item) { return push(end(), item); }
	inline NewList& pop(T& item) { return pop(first(), item); }
	inline T pop() { T item; pop(item); return item; }
	inline NewList& operator<<(const T& item) { return push(item); }
	inline NewList& operator>>(T& item) { return pop(item); }
	
	template<template<class> class CB>
	inline NewList& operator<<(CB<T>& cb) {
		while (!cb.empty()) {
			T item;
			cb >> item;
			*this << item;
		}
		return *this;
	}
	
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
	// inline T get(int j) const { return get(index(j)); }
	inline const T& at(int j) const { return at(index(j)); }
	// inline void set(int j, const T& item) { set(j, item); }
	
	// convenience methods
	inline void append(const T& item) { push(end(), item); }
	inline void insert(Index& index, const T& item) { push(index, item); }
	inline void remove(Index& index, T& item) { pop(index, item); }
	inline void remove(Index& index) { pop(index); }
	inline void pushFront(const T& item) { push(first(), item); }
	inline void pushBack(const T& item) { push(end(), item); }
	inline T popFront() { return pop(first()); }
	inline T popBack() { return pop(last()); }
	Index find(const T& item, Index index) const;
	inline Index find(const T& item) const { return find(item, first()); }
	int replace(const T& oldItem, const T& newItem);
	inline bool contains(const T& item) const { return def(find(item)); }
	
	// return value to reference conversion wrappers
	inline NewList& push(Return<Index> ret, const T& item) {
		Index index = ret;
		return push(index, item);
	}
	inline NewList& pop(Return<Index> ret, T& item) {
		Index index = ret;
		return pop(index, item);
	}
	inline T pop(Return<Index> ret) {
		Index index = ret;
		Item item;
		pop(ret, item);
		return item;
	}
	inline NewList& insert(Return<Index> ret, const T& item) {
		Index index = ret;
		return push(index, item);
	}
	inline NewList& remove(Return<Index> ret, T& item) {
		Index index = ret;
		return pop(index, item);
	}
	
private:
	friend class ListIterator<T>;
	typedef ListNode<T> Node;
	
	Node endNode_;
	Node* firstNode_;
	Node* lastNode_;
	int length_;
	int maxLength_;
};

template<class T>
NewList<T>::NewList(int maxLength)
	: firstNode_(0),
	  lastNode_(0),
	  length_(0),
	  maxLength_(maxLength)
{}

template<class T>
NewList<T>::~NewList()
{
	clear();
}

template<class T>
NewList<T>& NewList<T>::push(Index& index, const T& item)
{
	check(index.list_ == this);
	check(index.valid() || index.atEnd() || empty());
	
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
NewList<T>& NewList<T>::pop(Index& index, T& oldItem)
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
void NewList<T>::clear()
{
	while (last().valid())
		pop(last());
}

template<class T>
typename NewList<T>::Index NewList<T>::find(const T& item, Index index) const
{
	while (index.valid()) {
		if (at(index) == item) break;
		++index;
	}
	return index;
}

template<class T>
int NewList<T>::replace(const T& oldItem, const T& newItem)
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

} // namespace pona

#endif // PONA_NEWLIST_HPP
