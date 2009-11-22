/*
 * PrefixTree.hpp -- string lookup tree
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_PREFIXMAP_HPP
#define PONA_PREFIXMAP_HPP

#include "Tree.hpp"
#include "LineSink.hpp"
#include "List.hpp"

namespace pona
{

template<class Char, class Value>
class PrefixTree: public Tree
{
public:
	typedef PrefixTree Node;

	PrefixTree()
		: ch_(Char()),
		  defined_(false),
		  value_(Value())
	{}
	
	template<class Char2>
	inline void insert(const Char2* key, Value value = Value())
	{
		int keyLen = 0;
		while (key[keyLen] != 0) ++keyLen;
		insert(key, keyLen, value);
	}
	
	template<class Char2>
	void insert(const Char2* key, int keyLen, Value value)
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
	
	template<class Char2>
	inline bool lookup(const Char2* key, Value* value = 0)
	{
		int keyLen = 0;
		while (key[keyLen] != 0) ++keyLen;
		return lookup(key, keyLen, value);
	}
	
	template<class Char2>
	bool lookup(const Char2* key, int keyLen, Value* value = 0)
	{
		bool found = false;
		Ref<Node> node = this;
		while ((node) && keyLen > 0)
		{
			Ref<Node> parent = node;
			node = node->step(*key);
			if (node)
				if (node->defined_) {
					if (value) *value = node->value_;
					found = true;
				}
			++key;
			--keyLen;
		}
		return found;
	}
	
	template<class Media>
	bool lookup(Media* media, int i0, int* i1 = 0, Value* value = 0)
	{
		bool found = false;
		int i = i0;
		Ref<Node> node = this;
		while ((node) && media->def(i))
		{
			Ref<Node> parent = node;
			node = node->step(media->get(i++));
			if (node)
				if (node->defined_)
				{
					if (i1) *i1 = i;
					if (value) *value = node->value_;
					found = true;
				}
		}
		return found;
	}
	
	/*void print(Ref<LineSink> output) const
	{
		Ref<Node> node = firstLeaf();
		while (node)
		{
			Ref<Node> node2 = node;
			
			while (node2->parent()) {
				if (node2->defined_)
					node2->printNode(output);
				node2 = node2->parent();
			}
			
			node = node->nextLeaf();
		}
	}*/
	
protected:
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
	
	/*void printNode(Ref<LineSink> output) const
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
	}*/
	
protected:
	PrefixTree(Char ch)
		: ch_(ch),
		  defined_(false)
	{}
	
	Char ch_;
	bool defined_;
	Value value_;
};

} // namespace pona

#endif // PONA_PREFIXMAP_HPP
