/*
 * PrefixTree.hpp -- string lookup tree
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef FTL_PREFIXMAP_HPP
#define FTL_PREFIXMAP_HPP

#include "types.hpp"
#include "Tree.hpp"
#include "LineSink.hpp"

namespace ftl
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
	
	/** Insert a key-value mapping if no key-value mapping with the same key exists already.
	  * If currentValue is non-null the current value the giving key maps to is returned.
	  * The function returns true if the new key-value mapping was inserted successfully.
	  */
	template<class Char2>
	bool insert(const Char2* key, int keyLen, Value value, Value* currentValue = 0, bool caseSensitive = true)
	{
		return caseSensitive ?
			insertFiltered<Char2, Identity>(key, keyLen, value, currentValue) :
			insertFiltered<Char2, ToLower>(key, keyLen, value, currentValue);
	}
	
	/** Lookup the key-value mapping with the longest matching key.
	  * The function returns true if an exact match was found.
	  */
	template<class Char2>
	inline bool lookup(const Char2* key, int keyLen, Value* value = 0, bool caseSensitive = true) {
		return caseSensitive ?
			lookupFiltered<Char2, Identity>(key, keyLen, value) :
			lookupFiltered<Char2, ToLower>(key, keyLen, value);
	}
	
	/** Match given media to the longest key-value mapping of this tree.
	  * The media is read starting from position i0.
	  * The function returns true if any matching key was found.
	  * The value of the longest matching key is returned in '*value'.
	  * The terminal match position is given in '*i1'.
	  */
	template<class Media>
	inline bool match(Media* media, int i0 = 0, int* i1 = 0, Value* value = 0, bool caseSensitive = true) {
		return caseSensitive ?
			matchFiltered<Media, Identity>(media, i0, i1, value) :
			matchFiltered<Media, ToLower>(media, i0, i1, value);
	}
	
	// convenience wrapper
	inline bool lookup(const char* key, Value* value = 0, bool caseSensitive = true) {
		return lookup(key, str::len(key), value, caseSensitive);
	}
	
	// convenience wrapper
	inline bool insert(const char* key, Value value = Value(), Value* currentValue = 0, bool caseSensitive = true) {
		return insert(key, str::len(key), value, currentValue, caseSensitive);
	}
	
	// convenience wrapper, matches entire media
	template<class Media>
	bool match(Media* media, Value* value = 0)
	{
		int i1 = 0;
		if (match(media, 0, &i1, value))
			return !media->def(i1);
		return false;
	}
	
protected:
	PrefixTree(Char ch)
		: ch_(ch),
		  defined_(false)
	{}
	
	template<class Char2, template<class> class Filter>
	bool insertFiltered(const Char2* key, int keyLen, Value value, Value* currentValue = 0)
	{
		Ref<Node> node = this;
		while (keyLen > 0)
		{
			Ref<Node> parent = node;
			node = node->step<Filter>(*key);
			if (!node) {
				node = new Node(*key);
				parent->appendChild(node);
			}
			++key;
			--keyLen;
		}
		bool undefined = !node->defined_;
		if (undefined) {
			node->value_ = value;
			node->defined_ = true;
		}
		if (currentValue)
			*currentValue = node->value_;
		return undefined;
	}
	
	template<class Char2, template<class> class Filter>
	bool lookupFiltered(const Char2* key, int keyLen, Value* value = 0)
	{
		Ref<Node> node = this;
		while ((node) && keyLen > 0)
		{
			Ref<Node> parent = node;
			node = node->step<Filter>(*key);
			if (node)
				if (node->defined_)
					if (value)
						*value = node->value_;
			++key;
			--keyLen;
		}
		return node ? ((keyLen == 0) && (node->defined_)) : false;
	}
	
	template<class Media, template<class> class Filter>
	bool matchFiltered(Media* media, int i0 = 0, int* i1 = 0, Value* value = 0)
	{
		bool found = false;
		int i = i0;
		Ref<Node> node = this;
		while ((node) && media->def(i))
		{
			Ref<Node> parent = node;
			node = node->step<Filter>(media->get(i++));
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
	
	template<template<class> class Filter>
	inline Ref<Node> step(Char ch)
	{
		ch = Filter<Char>::map(ch);
		bool found = false;
		Ref<Node> node = firstChild();
		while (node) {
			found = (Filter<Char>::map(node->ch_) == ch);
			if (found) break;
			node = node->nextSibling();
		}
		
		return node;
	}
	
	Char ch_;
	bool defined_;
	Value value_;
};

} // namespace ftl

#endif // FTL_PREFIXMAP_HPP
