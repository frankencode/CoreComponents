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
#include "OrdinalTree.hpp"
#include "Pair.hpp"
#include "List.hpp"

namespace ftl
{

template<class Key, class Value>
class Map:
	public Container< Pair<Key, Value>, Map<Key, Value> >,
	public OrdinalTree< Pair<Key, Value> >
{
private:
	typedef OrdinalTree< Pair<Key,Value> > Super;
	typedef typename Super::Node Node;
	
public:
	typedef Pair<Key,Value> Item;
	typedef int Index;
	typedef GenericIterator< Map<Key, Value> > Iterator;
	
	typedef List<Item> ItemList;
	typedef List<Key> KeyList;
	typedef List<Value> ValueList;
	
	Map() {}
	
	inline Index first() const { return Super::firstIndex(); }
	inline Index last() const { return Super::lastIndex(); }
	inline Index end() const { return Super::lastIndex() + 1; }
	
	inline bool def(Index i) { return (0 <= i) && (i < Super::numberOfNodes()); }
	inline Item get(int i) const {
		Node* k = 0;
		return Super::lookupByIndex(i, &k) ? k->data() : Item();
	}
	
	/** Insert a key-value mapping if no key-value mapping with the same key exists already.
	  * If currentValue is non-null the current value the giving key maps to is returned.
	  * The function returns true if the new key-value mapping was inserted successfully.
	  */
	inline bool insert(const Key& key, const Value& value, Value* currentValue = 0, int* index = 0)
	{
		bool found = false;
		Item e(key, value);
		Node* k = find(e, &found, index);
		if (found) {
			if (currentValue)
				*currentValue = k->e_.value();
		}
		else {
			spliceIn(k, new Node(e));
		}
		return !found;
	}
	
	inline bool remove(const Key& key, int* index = 0)
	{
		bool found;
		Node* k = find(Item(key), &found, index);
		if (found) {
			Super::unlink(k);
			delete k;
		}
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
		Node* k = find(Item(key), &found, index);
		if (found && (value))
			*value = k->e_.value();
		return found;
	}
	
	/** Convenience wrapper to insert(), always overwrites an existing key value pair.
	  */
	inline int define(const Key& key, const Value& value) {
		Value oldValue;
		int index;
		insert(key, value, &oldValue, &index);
		return index;
	}
	
	/** Convenience wrapper to lookup()
	  */
	inline Value value(const Key& key) const {
		Value value = Value();
		lookup(key, &value);
		return value;
	}
	
	/** Associative operator
	  */
	inline Value operator[](const Key& key) const { return value(key); }
	inline Value& operator[](const Key& key) {
		bool found = false;
		Item e(key, Value());
		Node* k = find(e, &found);
		if (!found) {
			Node* kn = new Node(e);
			spliceIn(k, kn);
			k = kn;
		}
		return k->e_.value();
	}
	
	/** Convenience wrapper to lookup()
	  */
	inline bool contains(Key key) { return lookup(key); }
	
	/** Number of key-value pairs stored in this map
	  */
	inline int size() const { return Super::count(); }
	inline int length() const { return Super::count(); }
	
	Ref<ItemList, Owner> itemList()
	{
		Ref<ItemList, Owner> list = new ItemList;
		Node* node = Super::first();
		while (node) {
			list->append(node->e_);
			node = node->next();
		}
		return list;
	}
	
	Ref<KeyList, Owner> keyList()
	{
		Ref<KeyList, Owner> list = new KeyList;
		Node* node = Super::first();
		while (node) {
			list->append(node->e_.key());
			node = node->next();
		}
		return list;
	}
	
	Ref<ValueList, Owner> valueList()
	{
		Ref<ValueList, Owner> list = new ValueList;
		Node* node = Super::first();
		while (node) {
			list->append(node->e_.value());
			node = node->next();
		}
		return list;
	}
	
	inline Map& push(const Item& item)
	{
		bool found = false;
		Node* k = find(item, &found);
		if (found)
			k->e_ = item;
		else
			spliceIn(k, new Node(item));
		return *this;
	}
	
	inline Map& pop(Item& item)
	{
		check(!isEmpty());
		Node* k = Super::minNode();
		item = k->e_;
		Super::unlink(k);
		delete k;
		return *this;
	}
	
	inline Item pop() {
		Item item;
		pop(item);
		return item;
	}
	
	inline bool isEmpty() const { return Super::count() == 0; }
};

} // namespace ftl

#endif // FTL_MAP_HPP
