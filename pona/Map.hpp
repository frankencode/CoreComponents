/*
 * Map.hpp -- ordered map
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_MAP_HPP
#define PONA_MAP_HPP

#include "AvlTree.hpp"
#include "KeyValuePair.hpp"
#include "List.hpp"

namespace pona
{

template<class Key, class Value>
class Map: public AvlTree< KeyValuePair<Key,Value> >
{
public:
	typedef AvlTree< KeyValuePair<Key,Value> > Super;
	typedef typename Super::Node Node;
	
	typedef KeyValuePair<Key,Value> Pair;
	typedef List<Pair> KeyValueList;
	typedef List<Key> KeyList;
	typedef List<Value> ValueList;
	
	/** Insert a key-value pair. If 'oldValue' is null an existing mapping is preserved.
	  * If oldValue is non-null an existing mapping is overwritten and the old value returned.
	  * The function returns true if the new key-value pair was inserted successfully.
	  */
	inline bool insert(const Key& key, const Value& value, Value* oldValue = 0)
	{
		bool found = false, success = false;
		Pair e(key, value);
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
	
	inline void remove(const Key& key)
	{
		bool found;
		Node* k = find(Pair(key), &found);
		if (found)
		{
			Super::unlink(k);
			delete k;
		}
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
		Node* k = find(Pair(key), &found);
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
	inline int size() const { return BinaryTree<Pair>::count(); }
	
	Ref<KeyValueList, Owner> keyValueList()
	{
		Ref<List<Pair>, Owner> list = new KeyValueList;
		Node* node = Super::first();
		int i = 0;
		int n = Super::count();
		list->push(0, n);
		while (node) {
			list->set(i, node->e_);
			++i;
			node = node->next();
		}
		return list;
	}
	
	Ref<KeyList, Owner> keyList()
	{
		Ref<KeyList, Owner> list = new KeyList;
		Node* node = Super::first();
		int i = 0;
		int n = Super::count();
		list->push(0, n);
		while (node) {
			list->set(i, node->e_.key());
			++i;
			node = node->next();
		}
		return list;
	}
	
	Ref<ValueList, Owner> valueList()
	{
		Ref<ValueList, Owner> list = new ValueList;
		Node* node = Super::first();
		int i = 0;
		int n = Super::count();
		list->push(0, n);
		while (node) {
			list->set(i, node->e_.value());
			++i;
			node = node->next();
		}
		return list;
	}
};

} // namespace pona

#endif // PONA_MAP_HPP
