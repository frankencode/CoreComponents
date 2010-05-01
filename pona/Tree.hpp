/*
 * Tree.hpp -- tree structure and algorithms
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_TREE_HPP
#define PONA_TREE_HPP

#include "atoms"

namespace pona
{

class Tree: public Instance
{
public:
	typedef Tree Node;
	
	~Tree();
	
	inline Ref<Node> parent() const { return parent_; }
	inline Ref<Node> firstChild() const { return firstChild_; }
	inline Ref<Node> lastChild() const { return lastChild_; }
	inline Ref<Node> nextSibling() const { return nextSibling_; }
	inline Ref<Node> previousSibling() const { return previousSibling_; }
	
	inline void appendChild(Ref<Node> node) { insertChild(node, lastChild_); }
	
	void insertChild(Ref<Node> node, Ref<Node> previousSibling = 0);
	void appendAllChildrenOf(Ref<Node> node);
	void unlink();
	
	// iterating leafs
	Ref<Node> firstLeaf() const;
	Ref<Node> lastLeaf() const;
	Ref<Node> nextLeaf() const;
	Ref<Node> previousLeaf() const;
	
	// iterating all nodes
	inline Ref<Node> first() const { return firstLeaf(); }
	inline Ref<Node> last() const { return lastLeaf(); }
	inline Ref<Node> next() const { return (nextSibling_) ? nextSibling_->firstLeaf() : parent_; }
	inline Ref<Node> previous() const { return (previousSibling_) ? previousSibling_->lastLeaf() : parent_; }
	
	inline int countChildren() const {
		return (firstChild_) ? firstChild_->countSiblings() : 0;
	}
	
	int countSiblings() const;
	
	/** Duplicate a tree structure with optionally omitting nodes which do not
	  * meat a filter criteria.
	  */
	template<template<class> class Filter, class TreeType>
	static Ref<TreeType, Owner> duplicate(Ref<TreeType> original);
	
	/** Pass-through filter.
	  */
	template<class TreeType>
	class IncludeAll {
	public:
		inline static bool pass(Ref<TreeType>) { return true; }
	};
	
	/** Convenience method.
	  */
	template<class TreeType>
	inline static Ref<TreeType, Owner> duplicate(Ref<TreeType> original) {
		return duplicate<TreeType, IncludeAll>(original);
	}
	
private:
	Ref<Node, SetNull> parent_;
	Ref<Node, Owner> firstChild_;
	Ref<Node, Owner> lastChild_;
	Ref<Node, Owner> nextSibling_;
	Ref<Node, SetNull> previousSibling_;
};

template<template<class> class Filter, class TreeType>
Ref<TreeType, Owner> Tree::duplicate(Ref<TreeType> original)
{
	Ref<TreeType, Owner> newTree = new TreeType(*original);
	check(!newTree->firstChild_);
	Ref<TreeType> child = original->firstChild();
	while (child) {
		if (Filter<TreeType>::pass(child)) {
			Ref<TreeType, Owner> newChild = duplicate(child);
			newTree->appendChild(newChild);
		}
		child = child->nextSibling_;
	}
	return newTree;
}

} // namespace pona

#endif // PONA_TREE_HPP
