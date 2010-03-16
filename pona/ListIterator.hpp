/*
 * ListIterator.hpp -- double-linked list iterator
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_LISTITERATOR_HPP
#define PONA_LISTITERATOR_HPP

#include "Exception.hpp"
#include "ListNode.hpp"

namespace pona
{

template<class T>
class List;

template<class T>
class ListIterator {
public:
	// prefix increment / decrement
	inline ListIterator& operator++() {
		check(valid());
		node_ = node_->next_;
		return *this;
	}
	inline ListIterator& operator--() {
		check(valid() || (atEnd() && (!list_->empty())));
		node_ = atEnd() ? list_->lastNode_ : node_->previous_;
		return *this;
	}
	inline ListIterator& operator+=(int delta) {
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
	inline ListIterator& operator-=(int delta) {
		return (*this) += -delta;
	}
	
	// postfix increment / decrement
	inline ListIterator operator++(int) {
		ListIterator it = *this;
		++(*this);
		return it;
	}
	inline ListIterator operator--(int) {
		ListIterator it = *this;
		--(*this);
		return it;
	}
	
	inline ListIterator operator+(int delta) const {
		ListIterator it = *this;
		return it += delta;
	}
	inline ListIterator operator-(int delta) const {
		ListIterator it = *this;
		return it -= delta;
	}
	
	inline bool operator==(const ListIterator& b) const {
		check(list_ == b.list_);
		return node_ == b.node_;
	}
	inline bool operator!=(const ListIterator& b) const {
		check(list_ == b.list_);
		return node_ != b.node_;
	}
	
private:
	friend class List<T>;
	
	ListIterator(const ListNode<T>* node, const List<T>* list)
		: node_(const_cast<ListNode<T>*>(node)),
		  list_(const_cast<List<T>*>(list))
	{}
	
	inline bool valid() const { return node_; }
	inline bool atEnd() const { return &list_->endNode_ == node_; }
	
	ListNode<T>* node_;
	List<T>* list_;
};

} // namespace pona

#endif // PONA_LISTITERATOR_HPP
