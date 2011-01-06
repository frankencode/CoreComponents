/*
 * PrefixTree.hpp -- string lookup tree
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_PREFIXTREE_HPP
#define FTL_PREFIXTREE_HPP

#include "types.hpp"
#include "strings.hpp"
#include "Tree.hpp"
#include "PrefixTreeWalker.hpp"
#include "streams" // DEBUG

namespace ftl
{

template<class Char, class Value>
class PrefixTree;

template<class Char, class Value>
class PrefixTree: public Tree< PrefixTree<Char, Value> >
{
public:
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
	inline bool insert(const Char2* key, int keyLen, Value value, Value* currentValue = 0, bool caseSensitive = true) {
		return caseSensitive ?
			insertFiltered<Char2, Identity>(key, keyLen, value, currentValue) :
			insertFiltered<Char2, ToLower>(key, keyLen, value, currentValue);
	}
	
	/** Lookup the key-value mapping with the longest matching key.
	  * The function returns true if an exact match was found.
	  */
	template<class Char2>
	inline bool lookup(const Char2* key, int keyLen, Value* value = 0, bool caseSensitive = true) const {
		return caseSensitive ?
			lookupFiltered<Char2, Identity>(key, keyLen, value) :
			lookupFiltered<Char2, ToLower>(key, keyLen, value);
	}
	
	/** Remove the key-value mapping with the exactly matching key.
	  * If an exactly matching key-value pair is found, it is removed and
	  * the value is returned. In this case the function returns true.
	  */
	template<class Char2>
	inline bool remove(const Char2* key, int keyLen, Value* value = 0, bool caseSensitive = true) {
		return caseSensitive ?
			removeFiltered<Char2, Identity>(key, keyLen) :
			removeFiltered<Char2, ToLower>(key, keyLen);
	}
	
	/** Match given media to the longest key-value mapping of this tree.
	  * The media is read starting from position i0.
	  * The function returns true if a matching key was found.
	  * The value of the matching key is returned in '*value'.
	  * The terminal match position is given in '*i1'.
	  */
	template<class Media>
	inline bool match(Media* media, int i0 = 0, int* i1 = 0, Value* value = 0, bool caseSensitive = true) const {
		return caseSensitive ?
			matchFiltered<Media, Identity>(media, i0, i1, value) :
			matchFiltered<Media, ToLower>(media, i0, i1, value);
	}
	
	// convenience wrapper
	inline bool lookup(const char* key, Value* value = 0, bool caseSensitive = true) const {
		return lookup(key, str::len(key), value, caseSensitive);
	}
	
	// convenience wrapper
	inline bool insert(const char* key, Value value = Value(), Value* currentValue = 0, bool caseSensitive = true) {
		return insert(key, str::len(key), value, currentValue, caseSensitive);
	}
	
	// convenience wrapper
	inline bool remove(const char* key, Value* value = 0, bool caseSensitive = true) {
		return remove(key, str::len(key), value, caseSensitive);
	}
	
	// convenience wrapper, matches entire media
	template<class Media>
	bool match(Media* media, Value* value = 0) const
	{
		int i1 = 0;
		if (match(media, 0, &i1, value))
			return !media->def(i1);
		return false;
	}
	
	typedef PrefixTreeWalker<Char, Value> Index;
	typedef Array<Char, DeepCopyZeroTerminatedArray> Key;
	
	inline Index first() const { return Index(Parent::firstLeaf()); }
	inline Index last() const { return Index(Parent::lastLeaf()); }
	
	inline bool def(Index index) const { return index.valid(); }
	
	Ref<Key, Owner> key(Index index) const
	{
		check(def(index));
		int size = 0;
		{
			Ref<Node> node = index.node_;
			if (node) {
				while (node != this) {
					node = node->parent();
					++size;
				}
			}
		}
		Ref<Key, Owner> s = new Key(size);
		{
			int i = size;
			Ref<Node> node = index.node_;
			if (node) {
				while (node != this) {
					--i;
					s->set(i, node->ch_);
					node = node->parent();
				}
			}
		}
		return s;
	}
	
	inline Value value(Index index) const {
		check(def(index));
		return index.node_->value_;
	}
	
	template<class Char2>
	bool predict(const Char2* key, int keyLen, Index* first, Index* last, Index* common = 0) const
	{
		Ref<Node> node = this;
		while ((node) && (keyLen > 0)) {
			node = node->step<Identity>(*key);
			++key;
			--keyLen;
		}
		bool found = (node) ? node->hasChildren() : false;
		if (found) {
			*first = Index(node->firstLeaf());
			{
				Ref<Node> lastNode = node;
				while (!lastNode->defined_) lastNode = lastNode->lastChild();
				*last = Index(lastNode);
			}
			if (common) {
				Ref<Node> commonNode = node;
				while (commonNode->hasSingleChild()) {
					commonNode = commonNode->firstChild();
					if (commonNode->defined_) break;
				}
				*common = Index(commonNode);
			}
		}
		return found;
	}
	
	// convenience wrapper
	inline bool predict(const char* key, Index* first, Index* last, Index* common = 0) const {
		return predict(key, str::len(key), first, last, common);
	}
	
protected:
	typedef Tree< PrefixTree<Char, Value> > Parent;
	typedef PrefixTree Node;
	friend class PrefixTreeWalker<Char, Value>;
	
	PrefixTree(Char ch)
		: ch_(ch),
		  defined_(false)
	{}
	
	template<class Char2, template<class> class Filter>
	bool insertFiltered(const Char2* key, int keyLen, Value value, Value* currentValue = 0)
	{
		Ref<Node> node = this;
		while (keyLen > 0) {
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
	bool lookupFiltered(const Char2* key, int keyLen, Value* value = 0) const
	{
		Ref<Node> node = this;
		while ((node) && (keyLen > 0)) {
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
	
	template<class Char2, template<class> class Filter>
	bool removeFiltered(const Char* key, int keyLen, Value* value = 0) const
	{
		Ref<Node> node = this;
		while ((node) && (keyLen > 0)) {
			node = node->step<Filter>(*key);
			++key;
			--keyLen;
		}
		bool matchExact = (node) ? node->defined_ : false;
		if (matchExact) {
			if (value) *value = node->value_;
			node->defined_ = false;
			while (node) {
				if (node->defined_ || node->hasChildren()) break;
				Ref<Node> parent = node->parent();
				node->unlink();
				node = parent;
			}
		}
		return matchExact;
	}
	
	template<class Media, template<class> class Filter>
	bool matchFiltered(Media* media, int i0 = 0, int* i1 = 0, Value* value = 0) const
	{
		bool found = false;
		int i = i0;
		Ref<Node> node = this;
		while ((node) && media->def(i)) {
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
	inline Ref<Node> step(Char ch) const
	{
		ch = Filter<Char>::map(ch);
		bool found = false;
		Ref<Node> node = this->firstChild();
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

#endif // FTL_PREFIXTREE_HPP
