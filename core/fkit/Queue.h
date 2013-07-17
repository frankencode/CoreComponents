/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_QUEUE_H
#define FKIT_QUEUE_H

#include "containers.h"

namespace fkit
{

template<class T>
class QueueNode
{
public:
	QueueNode(const T &item)
		: item_(item),
		  next_(0),
		  prev_(0)
	{}
	T item_;
	QueueNode *next_;
	QueueNode *prev_;
};

template<class T>
class Queue: public Container< T, Queue<T> >
{
public:
	typedef T Item;

	inline static Ref<Queue> create() {
		return new Queue;
	}

	~Queue()
	{
		Node *node = head_;
		while (node) {
			Node *next = node->next_;
			delete node;
			node = next;
		}
	}

	inline int length() const { return length_; }
	inline bool isEmpty() const { return length_ == 0; }

	void pushBack(const T &item)
	{
		Node *node = new Node(item);
		if (tail_) {
			tail_->next_ = node;
			node->prev_ = tail_;
			tail_ = node;
		}
		else {
			head_ = node;
			tail_ = node;
		}
		++length_;
	}

	void pushFront(const T &item)
	{
		Node *node = new Node(item);
		if (head_) {
			head_->prev_ = node;
			node->next_ = head_;
			head_ = node;
		}
		else {
			head_ = node;
			tail_ = node;
		}
		++length_;
	}

	void popBack(T *item)
	{
		FKIT_ASSERT(length_ > 0);
		Node *node = tail_;
		if (item) *item = node->item_;
		tail_ = node->prev_;
		if (!tail_) head_ = 0;
		else tail_->next_ = 0;
		delete node;
		--length_;
	}

	void popFront(T *item)
	{
		FKIT_ASSERT(length_ > 0);
		Node *node = head_;
		if (item) *item = node->item_;
		head_ = node->next_;
		if (!head_) tail_ = 0;
		else head_->prev_ = 0;
		delete node;
		--length_;
	}

	inline T popBack() {
		T item;
		popBack(&item);
		return item;
	}

	inline T popFront() {
		T item;
		popFront(&item);
		return item;
	}

	inline void push(const T &item) { pushBack(item); }
	inline void pop(T *item) { return popFront(item); }
	inline T pop() { return popFront(); }

	inline T front() const { return head_->item_; }
	inline T back() const { return tail_->item_; }

private:
	typedef QueueNode<T> Node;

	Queue()
		: head_(0), tail_(0), length_(0)
	{}

	Node *head_;
	Node *tail_;
	int length_;
};

} // namespace fkit

#endif // FKIT_QUEUE_H
