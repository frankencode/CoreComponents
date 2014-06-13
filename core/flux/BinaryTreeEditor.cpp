/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "BinaryTreeEditor.h"

namespace flux
{

/** Attaches the new leaf node k under kp as the left or right child.
  */
void BinaryTreeEditor::attach(BinaryNode *kp, BinaryNode *k, bool left)
{
	if (kp) {
		if (left)
			kp->left_ = k;
		else
			kp->right_ = k;
		k->parent_ = kp;
		k->left_ = 0;
		k->right_ = 0;
	}
	else {
		setRoot(k);
		k->parent_ = 0;
		k->left_ = 0;
		k->right_ = 0;
	}
	changed(kp, k, left, true);
}

/** Detaches the leaf or list node k from the tree.
  */
BinaryNode *BinaryTreeEditor::detach(BinaryNode *k)
{
	BinaryNode *kp = k->parent_;
	bool left = false;
	if (kp) {
		BinaryNode *kc = (k->left_ != 0) ? k->left_ : k->right_;
		left = (kp->left_ == k);
		if (left)
			kp->left_ = kc;
		else
			kp->right_ = kc;
		if (kc != 0) kc->parent_ = kp;
	}
	else {
		setRoot(0);
	}
	changed(kp, k, left, false);
	return k;
}

/** Put the leaf node kl into the place of inner node ki
  */
void BinaryTreeEditor::replaceNode(BinaryNode *ki, BinaryNode *kl)
{
	kl->balance_ = ki->balance_;
	kl->weight_ = ki->weight_;

	// establish links to neighbors
	kl->parent_ = ki->parent_;
	kl->left_ = ki->left_;
	kl->right_ = ki->right_;

	// establish links from neighbors
	BinaryNode *kp = ki->parent_;
	if (kp) {
		if (kp->left_ == ki)
			kp->left_ = kl;
		else
			kp->right_ = kl;
	}
	else {
		setRoot(kl);
	}
	if (ki->left_)
		ki->left_->parent_ = kl;
	if (ki->right_)
		ki->right_->parent_ = kl;
}

} // namespace flux