/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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

	inline int count() const { return tree_.weight(); }

	inline bool has(int index) const {
		return 0 <= index && index < count();
	}
	inline Item &at(int index) const {
		Node *node = 0;
		if (!tree_.lookupByIndex(index, &node))
			FLUX_ASSERT(false);
		return node->item_;
	}

	inline Item& first() const { return at(0); }
	inline Item& last() const { return at(count() - 1); }

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

	inline void push(const Item &item) { push(count(), item); }
	inline void pop(Item *item) { pop(0, item); }
	inline Item pop() { Item item; pop(&item); return item; }

	inline void append(const Item &item) { push(count(), item); }
	inline void appendList(const List *b) { if (b) for (int i = 0; i < b->count(); ++i) append(b->at(i)); }
	inline void insert(int index, const Item &item) { push(index, item); }
	inline void remove(int index, Item &item) { pop(index, &item); }
	inline void remove(int index) { pop(index); }
	inline void pushFront(const Item &item) { push(0, item); }
	inline void pushBack(const Item &item) { push(count(), item); }
	inline Item popFront() { return pop(0); }
	inline Item popBack() { return pop(count() - 1); }

	int find(const Item &item, int index = 0) const
	{
		while (index < count()) {
			if (at(index) == item) break;
			++index;
		}
		return index;
	}
	int replace(const Item &oldItem, const Item &newItem)
	{
		int index = 0;
		int numReplaced = 0;
		while (index < count()) {
			if (at(index) == oldItem) {
				at(index) = newItem;
				++numReplaced;
			}
			++index;
		}
		return numReplaced;
	}
	inline bool contains(const Item &item) const { return find(item) < count(); }
	inline Item join(const Item &sep = Item()) const { return Item::join(this, sep); }

	Ref<List> sort(int order = SortOrder::Ascending, bool unique = false) const
	{
		if (count() == 0)
			return List::create();
		Ref< Heap<Item> > heap = Heap<Item>::create(count(), order);
		for (int i = 0; i < count(); ++i)
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
			result = List::create(count());
			for (int i = 0, n = result->count(); i < n; ++i)
				result->at(i) = heap->pop();
		}
		return result;
	}

	Ref<List> reverse() const
	{
		Ref<List> result = List::create(count());
		for (int i = 0, n = count(); i < n; ++i)
			result->at(i) = at(n - i - 1);
		return result;
	}

	Ref<List> unique(int order = SortOrder::Ascending) const { return sort(order, true); }

	template<class T2>
	Ref< List<T2> > toList() const {
		Ref< List<T2 > > result = List<T2>::create(count());
		for (int i = 0; i < count(); ++i)
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

} // namespace flux

#endif // FLUX_LIST_H
