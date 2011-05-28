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
class List: public Container<T, List<T> >, public Sequence<T, int>
{
public:
	typedef T Item;
	typedef GenericIterator<List> Iterator;
	
	inline Iterator iterator() const { return Iterator(this); }
	
	inline bool isEmpty() const { return tree_.weight() == 0; }
	inline int length() const { return tree_.weight(); }
	
	inline bool has(int index) const { return index < length(); }
	
	inline T get(int index) const {
		return at(index);
	}
	inline const T& at(int index) const {
		Node* node = 0;
		if (tree_.lookupByIndex(index, &node)) return node->e_;
		else return nullItem_;
	}
	inline void set(int index, const T& item) {
		Node* node = 0;
		if (tree_.lookupByIndex(index, &node))
			node->e_ = item;
	}
	
	inline List& push(int index, const T& item) {
		tree_.push(index, item);
		return *this;
	}
	inline List& pop(int index, T* item) {
		tree_.pop(index, item);
		return *this;
	}
	inline void clear() { tree_.clear(); }
	
	inline T pop(int index) {
		Item item;
		pop(index, &item);
		return item;
	}
	
	inline List& push(const T& item) { return push(length(), item); }
	inline List& pop(T* item) { return pop(0, item); }
	inline T pop() { T item; pop(&item); return item; }
	
	inline void append(const T& item) { push(length(), item); }
	inline void insert(int index, const T& item) { push(index, item); }
	inline void remove(int index, T& item) { pop(index, &item); }
	inline void remove(int index) { pop(index); }
	inline void pushFront(const T& item) { push(0, item); }
	inline void pushBack(const T& item) { push(length(), item); }
	inline T popFront() { return pop(0); }
	inline T popBack() { return pop(length() - 1); }
	
	int find(const T& item, int index) const
	{
		while (index < length()) {
			if (at(index) == item) break;
			++index;
		}
		return index;
	}
	int replace(const T& oldItem, const T& newItem)
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
	inline int find(const T& item) const { return find(item, 0); }
	inline bool contains(const T& item) const { return find(item) < length(); }
	inline T join(const T& sep = T()) const { return T::join(this, sep); }
	
	inline int index(int i) const { return i; }
	
	Ref<List, Owner> sort(int order = SortOrder::Ascending, bool unique = false) const
	{
		Ref<List, Owner> result = new List;
		if (length() == 0) return result;
		Heap<T> heap(length(), order);
		for (int i = 0; i < length(); ++i)
			heap.push(get(i));
		if (unique) {
			T prev, item;
			heap.read(&prev);
			result->append(prev);
			while (heap.read(&item)) {
				if (item != prev) {
					result->append(item);
					prev = item;
				}
			}
		}
		else {
		    T item;
			while (heap.read(&item))
				result->append(item);
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
