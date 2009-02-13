/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstractions Library.
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

#ifndef PONA_PREFIXMAP_HPP
#define PONA_PREFIXMAP_HPP

#include "Tree.hpp"
#include "LineSink.hpp"
#include "List.hpp"

namespace pona
{

template<class Char, class Value>
class PrefixMap: public Tree
{
public:
	typedef PrefixMap Node;

	PrefixMap()
		: ch_(Char()),
		  defined_(false),
		  value_(Value())
	{}
	
	template<class Char2>
	inline void define(const Char2* key, Value value = Value())
	{
		int len = 0;
		while (key[len] != 0) ++len;
		Char* s_ = new Char[len];
		for (int i = 0; i < len;  ++i)
			s_[i] = key[i];
		define(s_, len, value);
		delete[] s_;
	}
	
	inline void define(const Char* key, Value value = Value())
	{
		int len = 0;
		while (key[len] != 0) ++len;
		define(key, len, value);
	}
	
	void define(const Char* key, int keyLen, Value value)
	{
		Ref<Node> node = this;
		
		while (keyLen > 0)
		{
			Ref<Node> parent = node;
			node = node->step(*key);
			
			if (!node) {
				node = new Node(*key);
				parent->appendChild(node);
			}
			
			++key;
			--keyLen;
		}
		
		if (keyLen == 0)
		{
			node->defined_ = true;
			node->value_ = value;
		}
	}
	
	inline Ref<Node> step(Char ch)
	{
		bool found = false;
		Ref<Node> node = firstChild();
		while (node)
		{
			found = (node->ch_ == ch);
			if (found) break;
			node = node->nextSibling();
		}
		
		return node;
	}
	
	inline Char ch() const { return ch_; }
	inline bool defined() const { return defined_; }
	inline Value value() const { return value_; }
	
	template<class Media>
	bool match(Media* media, int i0, int* i1 = 0, Value* value = 0)
	{
		bool matched = false;
		int i = i0, n = media->length();
		Ref<Node> node = this;
		while ((node) && (i < n))
		{
			Ref<Node> parent = node;
			node = node->step(media->get(i++));
			if (node)
				if (node->defined())
				{
					if (i1) *i1 = i;
					if (value) *value = node->value();
					matched = true;
				}
		}
		return matched;
	}
	
	void print(Ref<LineSink> output) const
	{
		Ref<Node> node = firstLeaf();
		while (node)
		{
			Ref<Node> node2 = node;
			
			while (node2->parent()) {
				if (node2->defined())
					node2->printNode(output);
				node2 = node2->parent();
			}
			
			node = node->nextLeaf();
		}
	}
	
	void printNode(Ref<LineSink> output) const
	{
		Ref<Node> node = this;
		List<Char> cache;
		while (node->parent()) {
			cache.pushFront(node->ch_);
			node = node->parent();
		}
		while (cache.length() > 0)
			output->write(format("%%|") % cache.popFront());
		output->write("\n");
	}
	
private:
	PrefixMap(Char ch)
		: ch_(ch),
		  defined_(false)
	{}
	
	Char ch_;
	bool defined_;
	Value value_;
};

} // namespace pona

#endif // PONA_PREFIXMAP_HPP

