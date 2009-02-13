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

#ifndef PONA_TREE_HPP
#define PONA_TREE_HPP

#include <assert.h>
#include "Atoms.hpp"

namespace pona
{

class Tree: public Instance
{
public:
	typedef Tree Node;
	
	inline Ref<Node> parent() const { return parent_; }
	inline Ref<Node> firstChild() const { return firstChild_; }
	inline Ref<Node> lastChild() const { return lastChild_; }
	inline Ref<Node> nextSibling() const { return nextSibling_; }
	inline Ref<Node> previousSibling() const { return previousSibling_; }
	
	inline void appendChild(Ref<Node> node) { return insertChild(node, lastChild_); }
	
	void insertChild(Ref<Node> node, Ref<Node> previousSibling = 0)
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
	
	void appendAllChildrenOf(Ref<Node> node)
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
	
	inline void unlink()
	{
		if (previousSibling_)
			previousSibling_->nextSibling_ = nextSibling_;
		if (nextSibling_)
			nextSibling_->previousSibling_ = previousSibling_;
		if (parent_->firstChild_ == this)
			parent_->firstChild_ = parent_->firstChild_->nextSibling_;
		if (parent_->lastChild_ == this)
			parent_->lastChild_ = parent_->lastChild_->previousSibling_;
		parent_ = 0;
	}
	
	inline Ref<Node> firstLeaf()
	{
		Ref<Node> node = this;
		while (node->firstChild())
			node = node->firstChild();
		return node;
	}
	
	inline Ref<Node> nextLeaf()
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
	
	inline int countChildren() const
	{
		return (firstChild_) ? firstChild_->countSiblings() : 0;
	}
	
	inline int countSiblings() const
	{
		int n = 0;
		const Node* node = this;
		while (node) {
			node = node->nextSibling_;
			++n;
		}
		return n;
	}
	
private:
	Ref<Node, SetNull> parent_;
	Ref<Node, Owner> firstChild_;
	Ref<Node, Owner> lastChild_;
	Ref<Node, Owner> nextSibling_;
	Ref<Node, SetNull> previousSibling_;
};

} // namespace pona

#endif // PONA_TREE_HPP

