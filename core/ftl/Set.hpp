 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_SET_HPP
#define FTL_SET_HPP

#include "containers.hpp"
#include "OrdinalTree.hpp"

namespace ftl
{

template<class T>
class Set: public Container< T, Set<T> >, public Sequence<T, int>
{
public:
	typedef T Item;
	typedef GenericIterator<Set> Iterator;

	inline static O<Set> create() { return new Set; }
	virtual O<Set> clone() const { return new Set(*this); }

	inline O<Iterator> createIterator() const { return Iterator::create(this); }

	inline bool isEmpty() const { return tree_.weight() == 0; }
	inline int length() const { return tree_.weight(); }
	inline int size() const { return tree_.weight(); }

	inline bool has(int index) const {
		return index < length();
	}
	inline Item get(int index) const {
		return at(index);
	}
	inline const Item &at(int index) const {
		Node *node = 0;
		if (tree_.lookupByIndex(index, &node)) return node->item_;
		else return nullItem_;
	}

	/** Return the index of the first item greater or equal _a_
	  */
	inline int first(const Item &a) const { return tree_.first(a); }

	/** Return the index of the first item lower or equal _b_
	  */
	inline int last(const Item &b) const { return tree_.last(b); }

	/** Insert a new item if no item with the same value exists already.
	  * If currentItem is non-null the item with the same value is returned.
	  * The function returns true if the new item was inserted successfully.
	  */
	inline bool insert(const Item &item, Item *currentItem = 0, int *index = 0)
	{
		bool found = false;
		bool below = true;
		Node *k = tree_.find(item, &found, &below, index);
		if (found) {
			if (currentItem)
				*currentItem = k->item_;
		}
		else {
			tree_.attach(k, new Node(item), below);
		}
		return !found;
	}

	inline bool remove(const Item &item, int *index = 0)
	{
		bool found;
		Node *k = tree_.find(item, &found, 0, index);
		if (found)
			tree_.remove(k);
		return found;
	}

	inline bool contains(const Item &item)
	{
		bool found = false;
		tree_.find(item, &found);
		return found;
	}

	inline void push(const Item &item)
	{
		bool found = false;
		bool below = true;
		Node *k = tree_.find(item, &found, &below);
		if (found)
			k->item_ = item;
		else
			tree_.attach(k, new Node(item), below);
	}

	inline void pop(Item *item)
	{
		FTL_ASSERT(!isEmpty());
		Node *k = tree_.min();
		*item = k->item_;
		tree_.remove(k);
	}

	inline Item pop() {
		Item item;
		pop(&item);
		return item;
	}

	inline void clear() { tree_.clear(); }

private:
	typedef OrdinalTree< OrdinalNode<Item> > Tree;
	typedef typename Tree::Node Node;

	Set() {}
	Set(const Set &b): tree_(b.tree_) {}
	inline const Set &operator=(const Set &b) { tree_ = b.tree_; return *this; }

	Tree tree_;
	Item nullItem_;
};

} // namespace ftl

#endif // FTL_SET_HP
