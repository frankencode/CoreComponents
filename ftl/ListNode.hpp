/*
 * ListNode.hpp -- double-linked list node
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_LISTNODE_HPP
#define FTL_LISTNODE_HPP

namespace ftl
{

template<class T>
class ListWalker;

template<class T>
class List;

template<class T>
class ListNode: public Instance {
private:
	friend class ListWalker<T>;
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

} // namespace ftl

#endif // FTL_LISTNODE_HPP
