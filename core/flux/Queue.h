/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_QUEUE_H
#define FLUX_QUEUE_H

#include "containers.h"

namespace flux
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
class Queue: public Object
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

	void pushBack(const T &item, int = 0)
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

	void pushFront(const T &item, int = 0)
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

	T popBack(T *item)
	{
		FLUX_ASSERT(length_ > 0);
		T h;
		if (!item) item = &h;
		Node *node = tail_;
		*item = node->item_;
		tail_ = node->prev_;
		if (!tail_) head_ = 0;
		else tail_->next_ = 0;
		delete node;
		--length_;
		return *item;
	}

	T popFront(T *item)
	{
		FLUX_ASSERT(length_ > 0);
		T h;
		if (!item) item = &h;
		Node *node = head_;
		*item = node->item_;
		head_ = node->next_;
		if (!head_) tail_ = 0;
		else head_->prev_ = 0;
		delete node;
		--length_;
		return *item;
	}

	inline T popBack() {
		T item;
		return popBack(&item);
	}

	inline T popFront() {
		T item;
		return popFront(&item);
	}

	inline void push(const T &item, int = 0) { pushBack(item); }
	inline T pop(T *item) { return popFront(item); }
	inline T pop() { return popFront(); }

	inline int count() const { return length_; }

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

} // namespace flux

#endif // FLUX_QUEUE_H
