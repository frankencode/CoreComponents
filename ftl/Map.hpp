/*
 * Map.hpp -- ordered map
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_MAP_HPP
#define FTL_MAP_HPP

#include "containers.hpp"
#include "BinaryTree.hpp"

namespace ftl
{

template<class Key, class Value>
class Map: public Container< Pair<Key, Value>, Map<Key, Value> >, public Sequence<Pair<Key,Value>, int>
{
private:
	typedef BinaryTree< Pair<Key,Value> > Tree;
	typedef typename Tree::Node Node;
	
public:
	typedef Pair<Key,Value> Item;
	typedef GenericIterator<Map> Iterator;
	
	Map() {}
	
	Map(const Map& b): tree_(b.tree_) {}
	inline const Map& operator=(const Map& b) { tree_ = b.tree_; return *this; }
	
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
	inline const Item& at(int index) const {
		Node* node = 0;
		if (tree_.lookupByIndex(index, &node)) return node->item_;
		else return nullItem_;
	}
	
	/** Return the index of the first item greater or equal _a_
	  */
	inline int first(const Key& a) const { return tree_.first(a); }
	
	/** Return the index of the first item lower or equal _b_
	  */
	inline int last(const Key& b) const { return tree_.last(b); }
	
	/** Insert a key-value mapping if no key-value mapping with the same key exists already.
	  * If currentValue is non-null the current value the giving key maps to is returned.
	  * The function returns true if the new key-value mapping was inserted successfully.
	  */
	inline bool insert(const Key& key, const Value& value, Value* currentValue = 0, int* index = 0)
	{
		bool found = false;
		bool below = true;
		Item e(key, value);
		Node* k = tree_.find(e, &found, &below, index);
		if (found) {
			if (currentValue)
				*currentValue = k->item_.value();
		}
		else {
			tree_.spliceIn(k, new Node(e), below);
		}
		return !found;
	}
	
	inline bool remove(const Key& key, int* index = 0)
	{
		bool found;
		Node* k = tree_.find(Item(key), &found, 0, index);
		if (found)
			delete tree_.unlink(k);
		return found;
	}

	/** Lookup key-value pair by given key.
	  * If a matching key-value pair is found the value is returned in 'value' and the
	  * function returns with true. If a matching key-value pair could not be found
	  * 'value' is not set and the function returns false.
	  */
	template<class Value2>
	inline bool lookup(const Key& key, Value2* value = 0, int* index = 0) const
	{
		bool found = false;
		Node* k = tree_.find(Item(key), &found, 0, index);
		if (found && (value))
			*value = k->item_.value();
		return found;
	}
	
	/** Convenience wrapper to insert(), always overwrites an existing key value pair.
	  */
	inline int define(const Key& key, const Value& value)
	{
		Value oldValue;
		int index;
		insert(key, value, &oldValue, &index);
		return index;
	}
	
	/** Convenience wrapper to lookup()
	  */
	inline Value value(const Key& key) const
	{
		Value value = Value();
		lookup(key, &value);
		return value;
	}
	
	/** Readonly associative operator
	  */
	inline Value operator[](const Key& key) const { return value(key); }
	
	/** Writable associative operator
	  */
	inline Value& operator[](const Key& key)
	{
		bool found = false;
		bool below = true;
		Item e(key, Value());
		Node* k = tree_.find(e, &found, &below);
		if (!found) {
			Node* kn = new Node(e);
			tree_.spliceIn(k, kn, below);
			k = kn;
		}
		return k->item_.value();
	}
	
	/** Convenience wrapper to lookup()
	  */
	inline bool contains(Key key) { return tree_.lookup(key); }
	
	inline Map& push(const Item& item)
	{
		bool found = false;
		bool below = true;
		Node* k = tree_.find(item, &found, &below);
		if (found)
			k->item_ = item;
		else
			tree_.spliceIn(k, new Node(item), below);
		return *this;
	}
	
	inline Map& pop(Item* item)
	{
		FTL_ASSERT(!isEmpty());
		Node* k = tree_.minNode();
		*item = k->item_;
		delete tree_.unlink(k);
		return *this;
	}
	
	inline Item pop() {
		Item item;
		pop(&item);
		return item;
	}
	
	inline void clear() { tree_.clear(); }
	
	inline bool health() { return tree_.health(); }
	
protected:
	Tree tree_;
	Item nullItem_;
};

} // namespace ftl

#endif // FTL_MAP_HPP
