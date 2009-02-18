/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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
