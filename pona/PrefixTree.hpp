/*
 * PrefixTree.hpp -- string lookup tree
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_PREFIXMAP_HPP
#define PONA_PREFIXMAP_HPP

#include "types.hpp"
#include "Tree.hpp"
#include "LineSink.hpp"

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
			node = node->step<Identity>(*key);
			
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
	inline bool lookup(const char* key, Value* value = 0) {
		return lookup(key, pona::strlen(key), value);
	}
	
	// convenience wrapper
	inline bool insert(const char* key, Value value = Value(), Value* oldValue = 0) {
		return insert(key, pona::strlen(key), value, oldValue);
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
	bool lookupFiltered(const Char2* key, int keyLen, Value* value = 0)
	{
		bool found = false;
		Ref<Node> node = this;
		while ((node) && keyLen > 0)
		{
			Ref<Node> parent = node;
			node = node->step<Filter>(*key);
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

} // namespace pona

#endif // PONA_PREFIXMAP_HPP
