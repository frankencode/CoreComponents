/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "BinaryTree"

namespace flux
{

/** Attaches the new leaf node k under kp as the left or right child.
  */
void BinaryTree::attach(BinaryNode *kp, BinaryNode *k, bool left)
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
BinaryNode *BinaryTree::detach(BinaryNode *k)
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
void BinaryTree::replace(BinaryNode *ki, BinaryNode *kl)
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

/** Remove the node k from the tree.
  */
BinaryNode *BinaryTree::unlink(BinaryNode *k)
{
	if (k->left_)
		replace(k, detach(max(k->left_)));
	else if (k->right_)
		replace(k, detach(min(k->right_)));
	else
		detach(k);
	return k;
}

void BinaryTree::attachBefore(BinaryNode *kb, BinaryNode *kn)
{
	if (!kb)
		BinaryTree::attach(kb, kn, true);
	else if (kb->left_)
		BinaryTree::attach(max(kb->left_), kn, false);
	else
		BinaryTree::attach(kb, kn, true);
}

void BinaryTree::attachAfter(BinaryNode *ka, BinaryNode *kn)
{
	if (!ka)
		BinaryTree::attach(ka, kn, true);
	else if (ka->right_)
		BinaryTree::attach(min(ka->right_), kn, true);
	else
		BinaryTree::attach(ka, kn, false);
}

BinaryNode *BinaryTree::pred(BinaryNode *k)
{
	if (k->left_)
		return max(k->left_);
	BinaryNode *kp = k->parent_;
	while (kp) {
		if (k == kp->right_) break;
		k = kp;
		kp = kp->parent_;
	}
	return kp;
}

BinaryNode *BinaryTree::succ(BinaryNode *k)
{
	if (k->right_)
		return min(k->right_);
	BinaryNode *kp = k->parent_;
	while (kp) {
		if (k == kp->left_) break;
		k = kp;
		kp = kp->parent_;
	}
	return kp;
}

BinaryNode *BinaryTree::min(BinaryNode *k)
{
	BinaryNode *k2 = k;
	while (k) {
		k2 = k;
		k = k->left_;
	}
	return k2;
}

BinaryNode *BinaryTree::max(BinaryNode *k)
{
	BinaryNode *k2 = k;
	while (k) {
		k2 = k;
		k = k->right_;
	}
	return k2;
}

} // namespace flux
