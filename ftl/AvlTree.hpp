/*
 * AvlTree.hpp -- height balanced binary tree
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_AVLTREE_HPP
#define FTL_AVLTREE_HPP

#include "BinaryTree.hpp"

namespace ftl
{

template<class NodeType>
class AvlTree: public BinaryTree<NodeType>
{
public:
	typedef NodeType Node;
	
protected:
	void touched(Node* kp, bool left, bool attached);
	Node* restoreBalance(Node* k1);
};

template<class Node>
inline void AvlTree<Node>::touched(Node* kp, bool left, bool attached)
{
	if (!kp) return;
	
	int delta = 2 * left - 1;
	Node* k = kp;
	while (true) {
		k->balance_ += attached ? delta : -delta;
		if ((k->balance_ == 2) || (k->balance_ == -2))
			k = restoreBalance(k);
		if ( (attached ? (k->balance_ == 0) : (k->balance_ != 0)) ||
		     (!k->parent_) )
			break;
		delta = 2 * (k->parent_->left_ == k) - 1;
		k = k->parent_;
	}
}

template<class Node>
Node* AvlTree<Node>::restoreBalance(Node* k1)
{
	if (k1->balance_ == 2) {
		Node* k2 = k1->left_;
		if (k2->balance_ == -1) {
			// -- case Ia->III
			Node* k3 = k2->right_;
			k1->balance_ = -(k3->balance_ == 1);
			k2->balance_ = (k3->balance_ == -1);
			k3->balance_ = 0;
			rotate(k2, true);
			rotate(k1, false);
		}
		else {
			// -- case Ib, Ic
			k1->balance_ = (k2->balance_ <= 0);
			k2->balance_ = k2->balance_ - 1;
			rotate(k1, false);
		}
	}
	else {
		Node* k2 = k1->right_;
		if (k2->balance_ == 1) {
			// -- case IIc->IV
			Node* k3 = k2->left_;
			k1->balance_ = (k3->balance_ == -1);
			k2->balance_ = -(k3->balance_ == 1);
			k3->balance_ = 0;
			rotate(k2, false);
			rotate(k1, true);
		}
		else {
			// -- case IIa, IIb
			k1->balance_ = -(k2->balance_ >= 0);
			k2->balance_ = k2->balance_ + 1;
			rotate(k1, true);
		}
	}
	return k1->parent_;
}

} // namespace ftl

#endif // FTL_AVLTREE_HPP
