/*
 * Tree.hpp -- tree structure and algorithms
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef FTL_TREE_HPP
#define FTL_TREE_HPP

#include "atoms"

namespace ftl
{

class Tree: public Instance
{
public:
	typedef Tree Node;
	
	~Tree() { disbandChildren(); }
	
	inline Ref<Node> parent() const { return parent_; }
	inline Ref<Node> firstChild() const { return firstChild_; }
	inline Ref<Node> lastChild() const { return lastChild_; }
	inline Ref<Node> nextSibling() const { return nextSibling_; }
	inline Ref<Node> previousSibling() const { return previousSibling_; }
	
	inline void appendChild(Ref<Node> node) { insertChild(node, lastChild_); }
	
	void insertChild(Ref<Node> node, Ref<Node> previousSibling = 0);
	void appendAllChildrenOf(Ref<Node> node);
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
	inline Ref<Node> previous() const { return (previousSibling_) ? previousSibling_->lastLeaf() : parent(); }
	
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

} // namespace ftl

#endif // FTL_TREE_HPP
