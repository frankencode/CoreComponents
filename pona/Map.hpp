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
	typedef KeyValuePair<Key,Value> Element;
	typedef AvlTree<Element> Parent;
	typedef typename Parent::Node Node;
	
	inline void set(Key key, Value value)
	{
		bool found;
		Element e(key, value);
		Node* k = find(e, &found);
		if (found)
			k->e_ = e;
		else
			spliceIn(k, new Node(e));
	}
	
	inline Value get(Key key, bool* found = 0) const
	{
		bool found2;
		Value value;
		Node* k = find(Element(key), &found2);
		if (found2)
			value = k->e_.value();
		if (found)
			*found = found2;
		return value;
	}
	
	inline void del(Key key)
	{
		bool found;
		Node* k = find(Element(key), &found);
		if (found)
		{
			BinaryTree<Element>::unlink(k);
			delete k;
		}
	}

	inline bool contains(Key key)
	{
		bool found;
		find(Element(key), &found);
		return found;
	}
	
	inline int fill() const { return BinaryTree<Element>::count(); }
	
	inline Ref<List<Element>, Owner> toList()
	{
		Ref<List<Element>, Owner> list = new List<Element>;
		Node* node = BinaryTree<Element>::first();
		int i = 0;
		int n = BinaryTree<Element>::count();
		list->push(0, n);
		while (node) {
			list->set(i, node->e_);
			++i;
			node = node->next();
		}
		return list;
	}
};

} // namespace pona

#endif // PONA_MAP_HPP
