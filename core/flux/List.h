/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_LIST_H
#define FLUX_LIST_H

#include "containers.h"
#include "OrdinalTree.h"
#include "Heap.h"

namespace flux
{

template<class T>
class List: public Object
{
public:
	typedef T Item;

	inline static Ref<List> create() { return new List; }
	inline static Ref<List> create(int n) { return new List(n); }
	inline static Ref<List> clone(List *a) { return new List(*a); }

	inline int size() const { return tree_.weight(); }

	inline bool has(int index) const {
		return 0 <= index && index < size();
	}
	inline Item &at(int index) const {
		Node *node = 0;
		if (!tree_.lookupByIndex(index, &node))
			FLUX_ASSERT(false);
		return node->item_;
	}

	inline Item& first() const { return at(0); }
	inline Item& last() const { return at(size() - 1); }

	inline List &push(int index, const Item &item) {
		tree_.push(index, item);
		return *this;
	}
	inline List &pop(int index, Item *item) {
		tree_.pop(index, item);
		return *this;
	}
	inline void clear() { tree_.clear(); }

	inline Item pop(int index) {
		Item item;
		pop(index, &item);
		return item;
	}

	inline void push(const Item &item) { push(size(), item); }
	inline void pop(Item *item) { pop(0, item); }
	inline Item pop() { Item item; pop(&item); return item; }

	inline void append(const Item &item) { push(size(), item); }
	inline void append(List *b) { if (b) for (int i = 0; i < b->size(); ++i) append(b->at(i)); }
	inline void insert(int index, const Item &item) { push(index, item); }
	inline void remove(int index, Item &item) { pop(index, &item); }
	inline void remove(int index) { pop(index); }
	inline void pushFront(const Item &item) { push(0, item); }
	inline void pushBack(const Item &item) { push(size(), item); }
	inline Item popFront() { return pop(0); }
	inline Item popBack() { return pop(size() - 1); }

	int find(const Item &item, int index = 0) const
	{
		while (index < size()) {
			if (at(index) == item) break;
			++index;
		}
		return index;
	}
	int replace(const Item &oldItem, const Item &newItem)
	{
		int index = 0;
		int numReplaced = 0;
		while (index < size()) {
			if (at(index) == oldItem)
				set(index, newItem);
			++index;
			++numReplaced;
		}
		return numReplaced;
	}
	inline bool contains(const Item &item) const { return find(item) < size(); }
	inline Item join(const Item &sep = Item()) const { return Item::join(this, sep); }

	Ref<List> sort(int order = SortOrder::Ascending, bool unique = false) const
	{
		if (size() == 0)
			return List::create();
		Ref< Heap<Item> > heap = Heap<Item>::create(size(), order);
		for (int i = 0; i < size(); ++i)
			heap->push(at(i));
		Ref<List> result;
		if (unique) {
			result = List::create();
			Item prev, item;
			heap->pop(&prev);
			result->append(prev);
			while (!heap->isEmpty()) {
				heap->pop(&item);
				if (item != prev) {
					result->append(item);
					prev = item;
				}
			}
		}
		else {
			result = List::create(size());
			for (int i = 0, n = result->size(); i < n; ++i)
				result->at(i) = heap->pop();
		}
		return result;
	}

	Ref<List> reverse() const
	{
		Ref<List> result = List::create(size());
		for (int i = 0, n = size(); i < n; ++i)
			result->at(i) = at(n - i - 1);
		return result;
	}

	Ref<List> unique(int order = SortOrder::Ascending) const { return sort(order, true); }

	template<class T2>
	Ref< List<T2> > toList() const {
		Ref< List<T2 > > result = List<T2>::create(size());
		for (int i = 0; i < size(); ++i)
			result->at(i) = at(i);
		return result;
	}

	inline void operator<<(const T& item) { push(item); }
	inline void operator>>(T* item) { pop(item); }
	inline operator List *() { return this; }

protected:
	List() {}
	List(int n): tree_(n) {}

private:
	typedef OrdinalTree< OrdinalNode<Item> > Tree;
	typedef typename Tree::Node Node;

	explicit List(const List &b): tree_(b.tree_) {}
	const List &operator=(const List &b);

	Tree tree_;
	mutable Item nullItem_;
};

template<class T>
bool operator==(const List<T> &a, const List<T> &b) { return container::compare(a, b) == 0; }

template<class T>
bool operator!=(const List<T> &a, const List<T> &b) { return container::compare(a, b) != 0; }

template<class T>
bool operator<(const List<T> &a, const List<T> &b) { return container::compare(a, b) < 0; }

template<class T>
bool operator>(const List<T> &a, const List<T> &b) { return container::compare(a, b) > 0; }

template<class T>
bool operator<=(const List<T> &a, const List<T> &b) { return container::compare(a, b) <= 0; }

template<class T>
bool operator>=(const List<T> &a, const List<T> &b) { return container::compare(a, b) >= 0; }

} // namespace ft

#endif // FLUX_LIST_H
