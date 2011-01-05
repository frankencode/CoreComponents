/*
 * ListWalker.hpp -- double-linked list walker
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_LISTWALKER_HPP
#define FTL_LISTWALKER_HPP

#include "Exception.hpp"
#include "ListNode.hpp"

namespace ftl
{

template<class T>
class List;

template<class T>
class ListWalker {
public:
	// prefix increment / decrement
	inline ListWalker& operator++() {
		check(valid());
		node_ = node_->next_;
		if (!node_) node_ = &list_->endNode_;
		return *this;
	}
	inline ListWalker& operator--() {
		check(valid() || (atEnd() && (!list_->isEmpty())));
		node_ = atEnd() ? list_->lastNode_ : node_->previous_;
		return *this;
	}
	inline ListWalker& operator+=(int delta) {
		while (delta > 0) {
			++*this;
			--delta;
		}
		while (delta < 0) {
			--*this;
			++delta;
		}
		return *this;
	}
	inline ListWalker& operator-=(int delta) {
		return (*this) += -delta;
	}
	
	// postfix increment / decrement
	inline ListWalker operator++(int) {
		ListWalker it = *this;
		++(*this);
		return it;
	}
	inline ListWalker operator--(int) {
		ListWalker it = *this;
		--(*this);
		return it;
	}
	
	inline ListWalker operator+(int delta) const {
		ListWalker it = *this;
		return it += delta;
	}
	inline ListWalker operator-(int delta) const {
		ListWalker it = *this;
		return it -= delta;
	}
	
	inline bool operator==(const ListWalker& b) const {
		check(list_ == b.list_);
		return node_ == b.node_;
	}
	inline bool operator!=(const ListWalker& b) const {
		check(list_ == b.list_);
		return node_ != b.node_;
	}
	
private:
	friend class List<T>;
	
	ListWalker(Ref< ListNode<T> > node, Ref< List<T> > list)
		: node_(node),
		  list_(list)
	{}
	
	inline bool valid() const { return node_ && (node_ != &list_->endNode_); }
	inline bool atEnd() const { return &list_->endNode_ == node_; }
	
	Ref< ListNode<T> > node_;
	Ref< List<T> > list_;
};

} // namespace ftl

#endif // FTL_LISTWALKER_HPP
