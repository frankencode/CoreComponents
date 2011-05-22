/*
 * List.hpp -- ordered list
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_LIST_HPP
#define FTL_LIST_HPP

#include "containers.hpp"
#include "BinaryTree.hpp"
#include "Heap.hpp"

namespace ftl
{

template<class T>
class List: public Container<T, List<T> >
{
public:
	typedef T Item;
	typedef int Index;
	typedef GenericIterator<List> Iterator;
	
	inline Iterator iterator() const { return Iterator(this); }
	
	inline Index first() const { return 0; }
	inline Index last() const { return length() - 1; }
	inline Index end() const { return length(); }
	
	inline bool isEmpty() const { return tree_.weight() == 0; }
	inline int length() const { return tree_.weight(); }
	
	inline bool def(Index index) const {
		return ((0 <= index) && (index < tree_.weight()));
	}
	inline T get(Index index) const {
		return at(index);
	}
	inline const T& at(Index index) const {
		Node* node = 0;
		if (tree_.lookupByIndex(index, &node)) return node->e_;
		else return nullItem_;
	}
	inline void set(Index index, const T& item) {
		Node* node = 0;
		if (tree_.lookupByIndex(index, &node))
			node->e_ = item;
	}
	
	inline List& push(Index index, const T& item) {
		tree_.push(index, item);
		return *this;
	}
	inline List& pop(Index index, T* item) {
		tree_.pop(index, item);
		return *this;
	}
	inline void clear() { tree_.clear(); }
	
	inline T pop(Index index) {
		Item item;
		pop(index, &item);
		return item;
	}
	
	inline List& push(const T& item) { return push(end(), item); }
	inline List& pop(T* item) { return pop(first(), item); }
	inline T pop() { T item; pop(&item); return item; }
	
	inline void append(const T& item) { push(end(), item); }
	inline void insert(Index index, const T& item) { push(index, item); }
	inline void remove(Index index, T& item) { pop(index, &item); }
	inline void remove(Index index) { pop(index); }
	inline void pushFront(const T& item) { push(first(), item); }
	inline void pushBack(const T& item) { push(end(), item); }
	inline T popFront() { return pop(first()); }
	inline T popBack() { return pop(last()); }
	
	Index find(const T& item, Index index) const
	{
		while (def(index)) {
			if (at(index) == item) break;
			++index;
		}
		return index;
	}
	int replace(const T& oldItem, const T& newItem)
	{
		Index index = first();
		int numReplaced = 0;
		while (def(index)) {
			if (at(index) == oldItem)
				set(index, newItem);
			++index;
			++numReplaced;
		}
		return numReplaced;
	}
	inline Index find(const T& item) const { return find(item, first()); }
	inline bool contains(const T& item) const { return def(find(item)); }
	inline T join(const T& sep = T()) const { return T::join(this, sep); }
	
	inline Index index(int i) const { return i; }
	
	Ref<List, Owner> sort(int order = SortOrder::Ascending, bool unique = false) const
	{
		Ref<List, Owner> result = new List;
		if (length() == 0) return result;
		Heap<T> heap(length(), order);
		for (Index i = first(); def(i); ++i)
			heap.push(get(i));
		if (unique) {
			T prev = heap.next();
			result->append(prev);
			while (heap.hasNext()) {
				T item = heap.next();
				if (item != prev) {
					result->append(item);
					prev = item;
				}
			}
		}
		else {
			while (heap.hasNext())
				result->append(heap.next());
		}
		return result;
	}
	
	Ref<List, Owner> unique(int order = SortOrder::Ascending) const { return sort(order, true); }
	
	inline bool health() { return tree_.health(); }
	
protected:
	typedef BinaryTree<T> Tree;
	typedef typename Tree::Node Node;
	Tree tree_;
	Item nullItem_;
};

} // namespace ftl

#endif // FTL_LIST_HPP
