/*
 * ListNode.hpp -- double-linked list node
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_LISTNODE_HPP
#define PONA_LISTNODE_HPP

namespace pona
{

template<class T>
class ListIterator;

template<class T>
class List;

template<class T>
class ListNode {
private:
	friend class ListIterator<T>;
	friend class List<T>;
	
	ListNode()
		: previous_(0),
		  next_(0)
	{}
	
	ListNode(ListNode* previous, ListNode* next, const T& item)
		: previous_(previous),
		  next_(next),
		  item_(item)
	{
		if (previous) previous->next_ = this;
		if (next) next->previous_ = this;
	}
	
	~ListNode()
	{
		if (previous_) previous_->next_ = next_;
		if (next_) next_->previous_ = previous_;
	}
	
	ListNode* previous_;
	ListNode* next_;
	T item_;
};

} // namespace pona

#endif // PONA_LISTNODE_HPP
