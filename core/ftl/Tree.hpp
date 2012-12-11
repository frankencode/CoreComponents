/*
 * Tree.hpp -- n-ary tree
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_TREE_HPP
#define FTL_TREE_HPP

#include "atoms"

namespace ftl
{

template<class Node>
class Tree: public Instance
{
public:
	~Tree() { disbandChildren(); }

	inline Ref<Node> parent() const { return parent_; }
	inline Ref<Node> firstChild() const { return firstChild_; }
	inline Ref<Node> lastChild() const { return lastChild_; }
	inline Ref<Node> nextSibling() const { return nextSibling_; }
	inline Ref<Node> previousSibling() const { return previousSibling_; }

	inline void appendChild(Ref<Node> node) { insertChild(node, lastChild_); }

	void insertChild(Ref<Node> node, Ref<Node> previousSibling = 0);
	void appendAllChildrenOf(Ref<Node> node); // HACK?, better: adoptChildrenOf(...)
	void disbandChildren();
	void unlink();

	// iterating leafs
	Ref<Node> firstLeaf() const;
	Ref<Node> lastLeaf() const;
	Ref<Node> nextLeaf() const;
	Ref<Node> previousLeaf() const;

	// iterating all nodes
	inline Ref<Node> first() const { return firstLeaf(); }
	inline Ref<Node> last() const { return lastLeaf(); }
	inline Ref<Node> next() const { return (nextSibling_) ? nextSibling_->firstLeaf() : parent(); }

	inline int countChildren() const {
		return (firstChild_) ? firstChild_->countSiblings() : 0;
	}
	inline bool hasChildren() const { return firstChild_; }
	inline bool hasSingleChild() const { return (firstChild_) && (firstChild_ == lastChild_); }

	int countSiblings() const;

private:
	inline Node *me() { return FTL_CAST_FROM_TO(Tree<Node>, Node, this); }
	inline const Node *me() const { return FTL_CAST_FROM_TO(const Tree<Node>, const Node, this); }

	Ref<Node> parent_;
	Ref<Node, Owner> firstChild_;
	Ref<Node, Owner> lastChild_;
	Ref<Node, Owner> nextSibling_;
	Ref<Node> previousSibling_;
};

template<class Node>
void Tree<Node>::insertChild(Ref<Node> node, Ref<Node> previousSibling)
{
	if (node->parent_)
		unlink();
	if (previousSibling) {
		FTL_ASSERT(previousSibling->parent_ == this);
		node->nextSibling_ = previousSibling->nextSibling_;
		node->previousSibling_ = previousSibling;
		previousSibling->nextSibling_ = node;
		if (node->nextSibling_)
			node->nextSibling_->previousSibling_ = node;
		else
			lastChild_ = node;
	}
	else {
		if (firstChild_) {
			node->nextSibling_ = firstChild_;
			node->nextSibling_->previousSibling_ = node;
		}
		else {
			node->nextSibling_ = 0;
			lastChild_ = node;
		}
		node->previousSibling_ = 0;
		firstChild_ = node;
	}

	node->parent_ = me();
}

template<class Node>
void Tree<Node>::appendAllChildrenOf(Ref<Node> node)
{
	if (!node->firstChild_)
		return;
	if (lastChild_) {
		lastChild_->nextSibling_ = node->firstChild_;
		node->firstChild_->previousSibling_ = lastChild_;
		lastChild_ = node->lastChild_;
	}
	else {
		firstChild_ = node->firstChild_;
		lastChild_ = firstChild_;
	}
	Ref<Node> child = node->firstChild_;
	while (child) {
		child->parent_ = me();
		child = child->nextSibling_;
	}
	node->firstChild_ = 0;
	node->lastChild_ = 0;
}

template<class Node>
void Tree<Node>::disbandChildren()
{
	Ref<Node> node = lastChild_;
	while (node) {
		Ref<Node> next = node->previousSibling_;
		node->disbandChildren();
		node->nextSibling_ = 0;
		node->previousSibling_ = 0;
		node->parent_ = 0;
		node = next;
	}
	lastChild_ = 0;
	firstChild_ = 0;
}

template<class Node>
void Tree<Node>::unlink()
{
	Ref<Node, Owner> self = me();
	if (previousSibling_)
		previousSibling_->nextSibling_ = nextSibling_;
	if (nextSibling_)
		nextSibling_->previousSibling_ = previousSibling_;
	if (parent_) {
		if (parent_->firstChild_ == this)
			parent_->firstChild_ = nextSibling_;
		if (parent_->lastChild_ == this)
			parent_->lastChild_ = previousSibling_;
		parent_ = 0;
	}
	self = 0;
}

template<class Node>
Ref<Node> Tree<Node>::firstLeaf() const
{
	Ref<Node> node = me();
	while (node->firstChild())
		node = node->firstChild();
	return node;
}

template<class Node>
Ref<Node> Tree<Node>::lastLeaf() const
{
	Ref<Node> node = me();
	while (node->lastChild())
		node = node->lastChild();
	return node;
}

template<class Node>
Ref<Node> Tree<Node>::nextLeaf() const
{
	Ref<Node> node = me();

	if (node->nextSibling()) {
		node = node->nextSibling()->firstLeaf();
	}
	else {
		while (node->parent()) {
			node = node->parent();
			if (node->nextSibling()) {
				node = node->nextSibling()->firstLeaf();
				break;
			}
		}
		if (!node->parent())
			node = 0;
	}

	return node;
}

template<class Node>
Ref<Node> Tree<Node>::previousLeaf() const
{
	Ref<Node> node = me();

	if (node->previousSibling()) {
		node = node->previousSibling()->lastLeaf();
	}
	else {
		while (node->parent()) {
			node = node->parent();
			if (node->previousSibling()) {
				node = node->previousSibling()->lastLeaf();
				break;
			}
		}
		if (!node->parent())
			node = 0;
	}

	return node;
}

template<class Node>
int Tree<Node>::countSiblings() const
{
	int n = 0;
	const Node *node = me();
	while (node) {
		node = node->nextSibling_;
		++n;
	}
	return n;
}

} // namespace ftl

#endif // FTL_TREE_HPP
