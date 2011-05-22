/*
 * LegacyListNode.hpp -- double-linked list node
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_LEGACYLISTNODE_HPP
#define FTL_LEGACYLISTNODE_HPP

namespace ftl
{

template<class T>
class LegacyListWalker;

template<class T>
class LegacyList;

template<class T>
class LegacyListNode: public Instance {
private:
	friend class LegacyListWalker<T>;
	friend class LegacyList<T>;
	
	LegacyListNode()
		: previous_(0),
		  next_(0)
	{}
	
	LegacyListNode(LegacyListNode* previous, LegacyListNode* next, const T& item)
		: previous_(previous),
		  next_(next),
		  item_(item)
	{
		if (previous) previous->next_ = this;
		if (next) next->previous_ = this;
	}
	
	~LegacyListNode()
	{
		if (previous_) previous_->next_ = next_;
		if (next_) next_->previous_ = previous_;
	}
	
	LegacyListNode* previous_;
	LegacyListNode* next_;
	T item_;
};

} // namespace ftl

#endif // FTL_LEGACYLISTNODE_HPP
