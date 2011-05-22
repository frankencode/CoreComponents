/*
 * LegacyListWalker.hpp -- double-linked list walker
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_LEGACYLISTWALKER_HPP
#define FTL_LEGACYLISTWALKER_HPP

#include "Exception.hpp"
#include "LegacyListNode.hpp"

namespace ftl
{

template<class T>
class LegacyList;

template<class T>
class LegacyListWalker {
public:
	// prefix increment / decrement
	inline LegacyListWalker& operator++() {
		check(valid());
		node_ = node_->next_;
		if (!node_) node_ = &list_->endNode_;
		return *this;
	}
	inline LegacyListWalker& operator--() {
		check(valid() || (atEnd() && (!list_->isEmpty())));
		node_ = atEnd() ? list_->lastNode_ : node_->previous_;
		return *this;
	}
	inline LegacyListWalker& operator+=(int delta) {
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
	inline LegacyListWalker& operator-=(int delta) {
		return (*this) += -delta;
	}
	
	// postfix increment / decrement
	inline LegacyListWalker operator++(int) {
		LegacyListWalker it = *this;
		++(*this);
		return it;
	}
	inline LegacyListWalker operator--(int) {
		LegacyListWalker it = *this;
		--(*this);
		return it;
	}
	
	inline LegacyListWalker operator+(int delta) const {
		LegacyListWalker it = *this;
		return it += delta;
	}
	inline LegacyListWalker operator-(int delta) const {
		LegacyListWalker it = *this;
		return it -= delta;
	}
	
	inline bool operator==(const LegacyListWalker& b) const {
		check(list_ == b.list_);
		return node_ == b.node_;
	}
	inline bool operator!=(const LegacyListWalker& b) const {
		check(list_ == b.list_);
		return node_ != b.node_;
	}
	
private:
	friend class LegacyList<T>;
	
	LegacyListWalker(Ref< LegacyListNode<T> > node, Ref< LegacyList<T> > list)
		: node_(node),
		  list_(list)
	{}
	
	inline bool valid() const { return node_ && (node_ != &list_->endNode_); }
	inline bool atEnd() const { return &list_->endNode_ == node_; }
	
	Ref< LegacyListNode<T> > node_;
	Ref< LegacyList<T> > list_;
};

} // namespace ftl

#endif // FTL_LEGACYLISTWALKER_HPP
