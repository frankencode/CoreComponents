/*
 * PrefixTree.hpp -- string lookup tree
 *
 * Copyright (c) 2007-2010, Frank Mertens
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
	
	/** Convenience wrapper.
	  */
	template<class Char2>
	inline bool insert(const Char2* key, Value value = Value(), Value* oldValue = 0) {
		return insert(key, pona::strlen(key), value, oldValue);
	}
	
	/** Insert a key-value mapping. If oldValue is null an existing mapping is preserved.
	  * If oldValue is non-null an existing mapping is overwritten and the old value returned.
	  * The function returns true if the new key-value mapping was inserted successfully.
	  */
	template<class Char2>
	bool insert(const Char2* key, int keyLen, Value value, Value* oldValue = 0)
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
		
		bool success = false;
		
		if ((!node->defined_) || (oldValue))
		{
			if (oldValue)
				*oldValue = node->value_;
			node->defined_ = true;
			node->value_ = value;
			success = true;
		}
		
		return success;
	}
	
	/** Convenience wrapper.
	  */
	template<class Char2>
	inline bool lookup(const Char2* key, Value* value = 0)
	{
		return lookup(key, pona::strlen(key), value);
	}
	
	/** Lookup the key-value mapping with the longest matching key.
	  * The function returns true if an exact match was found.
	  */
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
		return found && (keyLen == 0);
	}
	
	/** Convenience wrapper, matches entire media.
	  */
	template<class Media>
	bool match(Media* media, Value* value = 0)
	{
		int i1 = 0;
		if (match(media, 0, &i1, value))
			return !media->def(i1);
		return false;
	}
	
	/** Match given media to the longest key-value mapping of this tree.
	  * The media is read starting from position i0.
	  * The function returns true if any matching key was found.
	  * The value of the longest matching key is returned in '*value'.
	  * The terminal match position is given in '*i1'.
	  */
	template<class Media>
	bool match(Media* media, int i0 = 0, int* i1 = 0, Value* value = 0)
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
	
protected:
	PrefixTree(Char ch)
		: ch_(ch),
		  defined_(false)
	{}
	
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
	
	Char ch_;
	bool defined_;
	Value value_;
};

} // namespace pona

#endif // PONA_PREFIXMAP_HPP
