/*
 * Tree.hpp -- tree structure and algorithms
 *
 * Copyright (c) 2007-2009, Frank Mertens
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
	
	Ref<Node> firstLeaf();
	Ref<Node> nextLeaf();
	
	inline int countChildren() const {
		return (firstChild_) ? firstChild_->countSiblings() : 0;
	}
	
	int countSiblings() const;
	
private:
	Ref<Node, SetNull> parent_;
	Ref<Node, Owner> firstChild_;
	Ref<Node, Owner> lastChild_;
	Ref<Node, Owner> nextSibling_;
	Ref<Node, SetNull> previousSibling_;
};

} // namespace pona

#endif // PONA_TREE_HPP
