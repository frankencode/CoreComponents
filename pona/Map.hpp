/*
 * Map.hpp -- ordered map
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_MAP_HPP
#define PONA_MAP_HPP

#include "AvlTree.hpp"
#include "Pair.hpp"
#include "NewList.hpp"

namespace pona
{

template<class Key, class Value>
class Map: public Instance, public AvlTree< Pair<Key,Value> >
{
private:	
	typedef AvlTree< Pair<Key,Value> > Super;
	typedef typename Super::Node Node;

public:
	typedef Pair<Key,Value> Item;
	typedef NewList<Item> ItemList;
	typedef NewList<Key> KeyList;
	typedef NewList<Value> ValueList;
	
	Map() {}
	
	/** Insert a key-value pair. If 'oldValue' is null an existing mapping is preserved.
	  * If oldValue is non-null an existing mapping is overwritten and the old value returned.
	  * The function returns true if the new key-value pair was inserted successfully.
	  */
	inline bool insert(const Key& key, const Value& value, Value* oldValue = 0)
	{
		bool found = false, success = false;
		Item e(key, value);
		Node* k = find(e, &found);
		if (found) {
			if (oldValue) {
				*oldValue = k->e_.value();
				k->e_ = e;
				success = true;
			}
		}
		else {
			spliceIn(k, new Node(e));
			success = true;
		}
		return success;
	}
	
	inline bool remove(const Key& key)
	{
		bool found;
		Node* k = find(Item(key), &found);
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
	inline bool lookup(const Key& key, Value2* value = 0) const
	{
		bool found = false;
		Node* k = find(Item(key), &found);
		if (found && (value))
			*value = k->e_.value();
		return found;
	}
	
	/** Convenience wrapper to insert(), always overwrites an existing key value pair.
	  */
	inline void set(const Key& key, const Value& value) {
		Value oldValue;
		insert(key, value, &oldValue);
	}
	
	/** Convenience wrapper to lookup()
	  */
	inline Value get(const Key& key) const {
		Value value = Value();
		lookup(key, &value);
		return value;
	}
	
	/** Convenience wrapper to lookup()
	  */
	inline bool contains(Key key) { return lookup(key); }
	
	/** Number of key-value pairs stored in this map
	  */
	inline int size() const { return Super::count(); }
	
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
		check(!empty());
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
	
	inline Map& operator<<(const Item& item) { return push(item); }
	inline Map& operator>>(Item& item) { return pop(item); }
	
	inline int empty() const { return Super::count() == 0; }
	
	template<template<class> class CB>
	inline Map& operator<<(CB<Item>& cb) {
		while (!cb.empty()) {
			Item item;
			cb >> item;
			*this << item;
		}
		return *this;
	}
	
	template<template<class> class CB>
	inline Map& operator>>(CB<Item>& cb) {
		while (!empty()) {
			Item item;
			*this >> item;
			cb << item;
		}
		return *this;
	}
};

} // namespace pona

#endif // PONA_MAP_HPP
