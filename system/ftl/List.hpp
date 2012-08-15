/*
 * List.hpp -- ordered list
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_LIST_HPP
#define FTL_LIST_HPP

#include "containers.hpp"
#include "OrdinalTree.hpp"
#include "Heap.hpp"

namespace ftl
{

template<class T>
class List: public Container< T, List<T> >, public Sequence<T, int>
{
public:
	typedef T Item;
	typedef GenericIterator<List> Iterator;

	inline static Ref<List, Owner> newInstance() { return new List; }
	inline static Ref<List, Owner> newInstance(int n) { return new List(n); }

	virtual Ref<List, Owner> clone() const { return new List(*this); }

	inline Iterator iterator() const { return Iterator(this); }

	inline bool isEmpty() const { return tree_.weight() == 0; }
	inline int length() const { return tree_.weight(); }
	inline int size() const { return tree_.weight(); }

	inline bool has(int index) const {
		return index < length();
	}
	inline Item get(int index) const {
		return at(index);
	}
	inline Item& at(int index) const {
		Node* node = 0;
		if (tree_.lookupByIndex(index, &node)) return node->item_;
		else return nullItem_;
	}
	inline void set(int index, const Item& item) {
		Node* node = 0;
		if (tree_.lookupByIndex(index, &node))
			node->item_ = item;
	}

	inline List& push(int index, const Item& item) {
		tree_.push(index, item);
		return *this;
	}
	inline List& pop(int index, Item* item) {
		tree_.pop(index, item);
		return *this;
	}
	inline void clear() { tree_.clear(); }

	inline Item pop(int index) {
		Item item;
		pop(index, &item);
		return item;
	}

	inline List& push(const Item& item) { return push(length(), item); }
	inline List& pop(Item* item) { return pop(0, item); }
	inline Item pop() { Item item; pop(&item); return item; }

	inline void append(const Item& item) { push(length(), item); }
	inline void insert(int index, const Item& item) { push(index, item); }
	inline void remove(int index, Item& item) { pop(index, &item); }
	inline void remove(int index) { pop(index); }
	inline void pushFront(const Item& item) { push(0, item); }
	inline void pushBack(const Item& item) { push(length(), item); }
	inline Item popFront() { return pop(0); }
	inline Item popBack() { return pop(length() - 1); }

	int find(const Item& item, int index = 0) const
	{
		while (index < length()) {
			if (at(index) == item) break;
			++index;
		}
		return index;
	}
	int replace(const Item& oldItem, const Item& newItem)
	{
		int index = 0;
		int numReplaced = 0;
		while (index < length()) {
			if (at(index) == oldItem)
				set(index, newItem);
			++index;
			++numReplaced;
		}
		return numReplaced;
	}
	inline bool contains(const Item& item) const { return find(item) < length(); }
	inline Item join(const Item& sep = Item()) const { return Item::join(this, sep); }

	Ref<List, Owner> sort(int order = SortOrder::Ascending, bool unique = false) const
	{
		if (length() == 0)
			return List::newInstance();
		Ref< Heap<Item>, Owner > heap = Heap<Item>::newInstance(length(), order);
		for (int i = 0; i < length(); ++i)
			heap->push(get(i));
		Ref<List, Owner> result;
		if (unique) {
			result = List::newInstance();
			Item prev, item;
			heap->read(&prev);
			result->append(prev);
			while (heap->read(&item)) {
				if (item != prev) {
					result->append(item);
					prev = item;
				}
			}
		}
		else {
			result = List::newInstance(length());
			Item item;
			for (int i = 0; heap->read(&item); ++i)
				result->set(i, item);
		}
		return result;
	}

	Ref<List, Owner> unique(int order = SortOrder::Ascending) const { return sort(order, true); }

	bool equals(Ref<List> b) const
	{
		if (length() != b->length()) return false;
		for (int i = 0; i < length(); ++i)
			if (at(i) != b->at(i)) return false;
		return true;
	}

	template<class T2>
	Ref<List<T2>, Owner> toList() const {
		Ref<List<T2>, Owner> result = List<T2>::newInstance(length());
		for (int i = 0; i < length(); ++i)
			result->set(i, at(i));
		return result;
	}

private:
	typedef OrdinalTree< OrdinalNode<Item> > Tree;
	typedef typename Tree::Node Node;

	List() {}
	List(int n): tree_(n) {}

	explicit List(const List& b): tree_(b.tree_) {}
	const List& operator=(const List& b);

	Tree tree_;
	mutable Item nullItem_;
};

template<
	class T,
	template<class> class GASP1,
	template<class> class GASP2
>
inline Ref<List<T>, Owner> operator+(Ref<List<T>, GASP1> a, Ref< List<T>, GASP2 > b)
{
	int n = a->length(), m = b->length();
	Ref< List<T>, Owner > r = List<T>::newInstance(n + m);
	int i = 0;
	for (;i < n; ++i) r->set(i, a->at(i));
	for (;i < n + m; ++i) r->set(i, b->at(i - n));
	return r;
}

} // namespace ftl

#endif // FTL_LIST_HPP
