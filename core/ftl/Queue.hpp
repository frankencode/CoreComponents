/*
 * Queue.hpp -- a single-linked queue
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_QUEUE_HPP
#define FTL_QUEUE_HPP

#include "containers.hpp"

namespace ftl
{

template<class T>
class QueueNode
{
public:
	QueueNode(const T &item)
		: item_(item),
		  next_(0)
	{}
	T item_;
	QueueNode *next_;
};

template<class T>
class Queue: public Container< T, Queue<T> >
{
public:
	typedef T Item;

	inline static Ref<Queue, Owner> create() {
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

	void push(const T &item)
	{
		Node *node = new Node(item);
		if (tail_) {
			tail_->next_ = node;
			tail_ = node;
		}
		else {
			head_ = node;
			tail_ = node;
		}
		++length_;
	}

	void pop(T *item)
	{
		FTL_ASSERT(length_ > 0);
		Node *node = head_;
		if (item) *item = node->item_;
		head_ = node->next_;
		if (!head_) tail_ = 0;
		delete node;
		--length_;
	}

	inline T pop() {
		T item;
		pop(&item);
		return item;
	}

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

} // namespace ftl

#endif // FTL_QUEUE_HPP
