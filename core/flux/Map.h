/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_MAP_H
#define FLUX_MAP_H

#include "containers.h"
#include "OrdinalTree.h"

namespace flux
{

template<class Key, class Value, class Mixin = None>
class Map: public Object, public Mixin
{
public:
	typedef Pair<Key,Value> Item;

	inline static Ref<Map> create() { return new Map(); }
	inline static Ref<Map> clone(Map *a) { return new Map(a); }

	inline int size() const { return tree_.weight(); }

	inline bool has(int index) const {
		return 0 <= index && index < size();
	}
	inline const Item &at(int index) const {
		Node *node = 0;
		if (!tree_.lookupByIndex(index, &node))
			FLUX_ASSERT(false);
		return node->item_;
	}

	inline const Key &keyAt(int index) const { return at(index).key(); }
	inline Value &valueAt(int index) const { return const_cast<Item &>(at(index)).value(); }

	/** Return the index of the first item greater or equal _a_
	  */
	inline int first(const Key &a) const { return tree_.first(a); }

	/** Return the index of the first item lower or equal _b_
	  */
	inline int last(const Key &b) const { return tree_.last(b); }

	/** Insert a key-value mapping if no key-value mapping with the same key exists already.
	  * If currentValue is non-null the current value the giving key maps to is returned.
	  * The function returns true if the new key-value mapping was inserted successfully.
	  */
	inline bool insert(const Key &key, const Value &value, Value *currentValue = 0, int *index = 0)
	{
		bool found = false;
		bool below = true;
		Item e(key, value);
		Node *k = tree_.find(e, &found, &below, index);
		if (found) {
			if (currentValue)
				*currentValue = k->item_.value();
		}
		else {
			tree_.attach(k, new Node(e), below);
		}
		return !found;
	}

	inline bool remove(const Key &key, int *index = 0)
	{
		bool found;
		Node *k = tree_.find(Item(key), &found, 0, index);
		if (found)
			tree_.remove(k);
		return found;
	}

	/** Insert or overwrite a key-value mapping.
	  */
	inline void establish(const Key &key, const Value &value) {
		int index = 0;
		if (!insert(key, value, 0, &index))
			valueAt(index) = value;
	}

	/** Lookup key-value pair by given key.
	  * If a matching key-value pair is found the value is returned in 'value' and the
	  * function returns with true. If a matching key-value pair could not be found
	  * 'value' is not set and the function returns false.
	  */
	template<class Value2>
	inline bool lookup(const Key &key, Value2 *value = 0, int *index = 0) const
	{
		bool found = false;
		Node *k = tree_.find(Item(key), &found, 0, index);
		if (found && (value))
			*value = k->item_.value();
		return found;
	}

	/** Retrieve value of an existing key-value mapping.
	  */
	inline Value value(const Key &key, const Value &fallback = Value()) const
	{
		Value value = fallback;
		lookup(key, &value);
		return value;
	}

	/** Set value of an existing key-value mapping.
	  */
	inline bool setValue(const Key &key, const Value &value)
	{
		int index = 0;
		bool found = lookup(key, (Value *)0, &index);
		if (found)
			valueAt(index) = value;
		return found;
	}

	/** Convenience wrapper to lookup()
	  */
	inline bool contains(const Key &key) const { return lookup<Value>(key); }

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
		FLUX_ASSERT(size() > 0);
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

	inline void operator<<(const Item &item) { push(item); }
	inline operator Map *() { return this; }

protected:
	typedef OrdinalTree< OrdinalNode<Item> > Tree;
	typedef typename Tree::Node Node;

	Map() {}
	explicit Map(const Map &b): tree_(b.tree_) {}

	const Map &operator=(const Map &b);

	Tree tree_;
	mutable Item nullItem_;
};

} // namespace flux

#endif // FLUX_MAP_H
