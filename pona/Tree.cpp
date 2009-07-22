/*
 * Tree.cpp -- tree structure and algorithms
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <assert.h>
#include "Tree.hpp"

namespace pona
{

void Tree::insertChild(Ref<Node> node, Ref<Node> previousSibling)
{
	if (node->parent_)
		unlink();
	if (previousSibling) {
		assert(previousSibling->parent_ == this);
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
	
	node->parent_ = this;
}

void Tree::appendAllChildrenOf(Ref<Node> node)
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
		child->parent_ = this;
		child = child->nextSibling_;
	}
	node->firstChild_ = 0;
	node->lastChild_ = 0;
}

void Tree::unlink()
{
	Ref<Node, Owner> self = this;
	if (previousSibling_)
		previousSibling_->nextSibling_ = nextSibling_;
	if (nextSibling_)
		nextSibling_->previousSibling_ = previousSibling_;
	if (parent_) {
		if (parent_->firstChild_ == this)
			parent_->firstChild_ = parent_->firstChild_->nextSibling_;
		if (parent_->lastChild_ == this)
			parent_->lastChild_ = parent_->lastChild_->previousSibling_;
		parent_ = 0;
	}
	self = 0;
}

Ref<Tree::Node> Tree::firstLeaf()
{
	Ref<Node> node = this;
	while (node->firstChild())
		node = node->firstChild();
	return node;
}

Ref<Tree::Node> Tree::nextLeaf()
{
	Ref<Node> node = this;
	
	if (node->nextSibling())
	{
		node = node->nextSibling()->firstLeaf();
	}
	else
	{
		while (node->parent())
		{
			node = node->parent();
			if (node->nextSibling())
			{
				node = node->nextSibling()->firstLeaf();
				break;
			}
		}
		if (!node->parent())
			node = 0;
	}
		
	return node;
}

int Tree::countSiblings() const
{
	int n = 0;
	const Node* node = this;
	while (node) {
		node = node->nextSibling_;
		++n;
	}
	return n;
}

} // namespace pona
