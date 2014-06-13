/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_BINARYTREE_H
#define FLUX_BINARYTREE_H

#include "BinaryNode.h"
#include "AvlBalance.h"
#include "BinaryTreeEditor.h"

namespace flux
{

template<class NodeType>
class BinaryTree: public AvlBalance, public BinaryTreeEditor
{
public:
	typedef NodeType Node;

	BinaryTree(): root_(0) {}
	virtual ~BinaryTree() { clear(); }

	inline Node *min() const { return static_cast<Node *>(BinaryTreeEditor::min(root_)); }
	inline Node *max() const { return static_cast<Node *>(BinaryTreeEditor::max(root_)); }

	inline void remove(Node *k) { return delete static_cast<Node *>(BinaryTreeEditor::removeNode(k)); }

	static Node *clone(Node *k);
	static void clear(Node *k);
	void clear();

	inline int weight() const { return BinaryTreeEditor::weight(root_); }

protected:
	inline void setRoot(BinaryNode *k) { root_ = static_cast<Node *>(k); }

	inline virtual void cleared() {}

#ifndef NDEBUG
	static bool testStructure(BinaryNode *k);
	static bool testWeight(BinaryNode *k);
	// static bool testIteration(Node *k);
	// static bool testOrder(Node *k);
#endif

	Node *root_;
};

template<class Node>
Node *BinaryTree<Node>::clone(Node *k)
{
	if (!k) return 0;
	Node *kn = new Node(*k);
	if (!k->parent_) kn->parent_ = 0;
	if (k->left_) {
		kn->left_ = clone(k->left());
		kn->left_->parent_ = kn;
	}
	else {
		kn->left_ = 0;
	}
	if (k->right_) {
		kn->right_ = clone(k->right());
		kn->right_->parent_ = kn;
	}
	else {
		kn->right_ = 0;
	}
	return kn;
}

template<class Node>
void BinaryTree<Node>::clear()
{
	clear(root_);
	root_ = 0;
	cleared();
}

template<class Node>
void BinaryTree<Node>::clear(Node *k)
{
	if (!k) return;
	clear(k->left());
	clear(k->right());
	delete k;
}

#ifndef NDEBUG

template<class Node>
bool BinaryTree<Node>::testStructure(BinaryNode *k)
{
	if (!k) return true;
	if (k->parent_) {
		if (!((k == k->parent_->left_) || (k == k->parent_->right_)))
			return false;
	}
	return testStructure(k->left_) && testStructure(k->right_);
}

template<class Node>
bool BinaryTree<Node>::testWeight(BinaryNode *k)
{
	if (!k) return true;
	return
		(weight(k->left_) + weight(k->right_) + 1 == k->weight_) &&
		testWeight(k->left_) && testWeight(k->right_);
}

/*template<class Node>
bool BinaryTree<Node>::testIteration(Node *k)
{
	if (k == 0) return true;
	Node *k2;
	k2 = succ(k);
	if (k2) {
		if (k != pred(k2))
			return false;
	}
	k2 = pred(k);
	if (k2) {
		if (k != succ(k2))
			return false;
	}
	return testIteration(k->left_) && testIteration(k->right_);
}

template<class Node>
bool BinaryTree<Node>::testOrder(Node *k)
{
	if (!k) return true;
	if (k->left_) {
		if (!(k->left_->item_ < k->item_))
			return false;
	}
	if (k->right_) {
		if (!(k->item_ < k->right_->item_))
			return false;
	}
	return testOrder(k->left_) && testOrder(k->right_);
}*/

#endif // ndef NDEBUG

} // namespace flux

#endif // FLUX_BINARYTREE_HP
