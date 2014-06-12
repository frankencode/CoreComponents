/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_AVLTREE_H
#define FLUX_AVLTREE_H

#include "BinaryTree.h"

namespace flux
{

template<class NodeType>
class AvlTree: public BinaryTree<NodeType>
{
public:
	typedef NodeType Node;

protected:
	virtual void touched(BinaryNode *kp, BinaryNode*, bool left, bool attached);
	BinaryNode *restoreBalance(BinaryNode *k1);
#ifndef NDEBUG
	static int height(BinaryNode *k);
	static bool testBalance(BinaryNode *k);
#endif
};

template<class Node>
inline void AvlTree<Node>::touched(BinaryNode *kp, BinaryNode*, bool left, bool attached)
{
	if (!kp) return;

	int delta = 2 * left - 1;
	BinaryNode *k = kp;
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
BinaryNode *AvlTree<Node>::restoreBalance(BinaryNode *k1)
{
	if (k1->balance_ == 2) {
		BinaryNode *k2 = k1->left_;
		if (k2->balance_ == -1) {
			// -- case Ia->III
			BinaryNode *k3 = k2->right_;
			k1->balance_ = -(k3->balance_ == 1);
			k2->balance_ = (k3->balance_ == -1);
			k3->balance_ = 0;
			this->rotate(k2, true);
			this->rotate(k1, false);
		}
		else {
			// -- case Ib, Ic
			k1->balance_ = (k2->balance_ <= 0);
			k2->balance_ = k2->balance_ - 1;
			this->rotate(k1, false);
		}
	}
	else {
		BinaryNode *k2 = k1->right_;
		if (k2->balance_ == 1) {
			// -- case IIc->IV
			BinaryNode *k3 = k2->left_;
			k1->balance_ = (k3->balance_ == -1);
			k2->balance_ = -(k3->balance_ == 1);
			k3->balance_ = 0;
			this->rotate(k2, false);
			this->rotate(k1, true);
		}
		else {
			// -- case IIa, IIb
			k1->balance_ = -(k2->balance_ >= 0);
			k2->balance_ = k2->balance_ + 1;
			this->rotate(k1, true);
		}
	}
	return k1->parent_;
}

#ifndef NDEBUG

template<class Node>
int AvlTree<Node>::height(BinaryNode *k)
{
	if (k == 0) return 0;
	int h1 = height(k->left_);
	int h2 = height(k->right_);
	return (h1 < h2 ? h2 : h1) + 1;
}

template<class Node>
bool AvlTree<Node>::testBalance(BinaryNode *k)
{
	if (!k) return true;
	if (!((k->balance_ == -1) || (k->balance_ == 0) || (k->balance_ == 1))) return false;
	if ((height(k->left_) - height(k->right_)) != k->balance_) return false;
	return testBalance(k->left_) && testBalance(k->right_);
}

#endif // ndef NDEBUG

} // namespace flux

#endif // FLUX_AVLTREE_H
