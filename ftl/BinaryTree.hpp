/*
 * BinaryTree.hpp -- binary tree
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_BINARYTREE_HPP
#define FTL_BINARYTREE_HPP

#include "atoms"

namespace ftl
{

template<class NodeType>
class BinaryTree
{
public:
	typedef NodeType Node;
	
	BinaryTree(): root_(0) {}
	virtual ~BinaryTree() { clear(); }
	
	inline Node* root() const { return root_; }
	
	void attach(Node* kp, Node* kn, bool left);
	Node* detach(Node* k);
	void replaceNode(Node* ki, Node* kl);
	void rotate(Node* k1, bool left);
	
	inline Node* min() const { return min(root_); }
	inline Node* max() const { return max(root_); }
	static Node* min(Node* k);
	static Node* max(Node* k);
	static Node* pred(Node* k);
	static Node* succ(Node* k);
	static Node* pred(Node* k, int delta);
	static Node* succ(Node* k, int delta);
	
	void attachBefore(Node* kb, Node* kn);
	void attachAfter(Node* ka, Node* kn);
	
	static Node* clone(Node* k);
	static void clear(Node* k);
	void clear();
	
	void remove(Node* k);
	
protected:
	inline virtual void touched(Node* kp, bool left, bool attached) {}
	inline virtual void rotated(Node* k1, bool left) {}
	inline virtual void cleared() {}
	
#ifndef NDEBUG
	static bool testStructure(Node* k);
	static bool testOrder(Node* k);
	static bool testIteration(Node* k);
#endif
	
	Node* root_;
};

/** Attaches the new leaf node kn under kp as the left or right child.
  */
template<class Node>
inline void BinaryTree<Node>::attach(Node* kp, Node* kn, bool left)
{
	if (kp) {
		if (left)
			kp->left_ = kn;
		else
			kp->right_ = kn;
		kn->parent_ = kp;
		kn->left_ = 0;
		kn->right_ = 0;
	}
	else {
		root_ = kn;
		kn->parent_ = 0;
		kn->left_ = 0;
		kn->right_ = 0;
	}
	touched(kp, left, true);
}

/** Detaches the leaf or list node k from the tree.
  */
template<class Node>
inline Node* BinaryTree<Node>::detach(Node* k)
{
	Node* kp = k->parent_;
	bool left = false;
	if (kp) {
		Node* kc = (k->left_ != 0) ? k->left_ : k->right_;
		left = (kp->left_ == k);
		if (left)
			kp->left_ = kc;
		else
			kp->right_ = kc;
		if (kc != 0) kc->parent_ = kp;
	}
	else {
		root_ = 0;
	}
	touched(kp, left, false);
	return k;
}

/** Put the leaf node kl into the place of inner node ki
  */
template<class Node>
void BinaryTree<Node>::replaceNode(Node* ki, Node* kl)
{
	kl->balance_ = ki->balance_;
	kl->weight_ = ki->weight_;
	
	// establish links to neighbors
	kl->parent_ = ki->parent_;
	kl->left_ = ki->left_;
	kl->right_ = ki->right_;
	
	// establish links from neighbors
	Node* kp = ki->parent_;
	if (kp) {
		if (kp->left_ == ki)
			kp->left_ = kl;
		else
			kp->right_ = kl;
	}
	else
		root_ = kl;
	if (ki->left_)
		ki->left_->parent_ = kl;
	if (ki->right_)
		ki->right_->parent_ = kl;
}

/** Perform an equivalent tree transformation.
  *
  * Rotate left:
  *
  *    k1      =>      k2
  *   /  \            /  \
  *  a    k2         k1   c
  *      /  \       /  \
  *     b    c     a    b
  *
  * Rotate right:
  *
  *      k1    =>    k2
  *     /  \        /  \
  *    k2   c      a    k1
  *   /  \             /  \
  *  a    b           b    c
  */
template<class Node>
void BinaryTree<Node>::rotate(Node* k1, bool left)
{
	Node* k2 = left ? k1->right_ : k1->left_;
	if (k1->parent_) {
		if (k1->parent_->left_ == k1)
			k1->parent_->left_ = k2;
		else
			k1->parent_->right_ = k2;
	}
	else {
		root_ = k2;
	}
	k2->parent_ = k1->parent_;
	k1->parent_ = k2;
	if (left) {
		k1->right_ = k2->left_;
		if (k2->left_)
			k2->left_->parent_ = k1;
		k2->left_ = k1;
	}
	else {
		k1->left_ = k2->right_;
		if (k2->right_)
			k2->right_->parent_ = k1;
		k2->right_ = k1;
	}
	rotated(k1, left);
}

template<class Node>
inline Node* BinaryTree<Node>::min(Node* k)
{
	Node* k2 = k;
	while (k) {
		k2 = k;
		k = k->left_;
	}
	return k2;
}

template<class Node>
inline Node* BinaryTree<Node>::max(Node* k)
{
	Node* k2 = k;
	while (k) {
		k2 = k;
		k = k->right_;
	}
	return k2;
}

template<class Node>
Node* BinaryTree<Node>::pred(Node* k)
{
	if (k->left_)
		return max(k->left_);
	Node* kp = k->parent_;
	while (kp) {
		if (k == kp->right_) break;
		k = kp;
		kp = kp->parent_;
	}
	return kp;
}

template<class Node>
Node* BinaryTree<Node>::succ(Node* k)
{
	if (k->right_)
		return min(k->right_);
	Node* kp = k->parent_;
	while (kp) {
		if (k == kp->left_) break;
		k = kp;
		kp = kp->parent_;
	}
	return kp;
}

template<class Node>
inline Node* BinaryTree<Node>::pred(Node* k, int delta)
{
	while ((delta > 0) && (k)) {
		k = pred(k);
		--delta;
	}
	return k;
}

template<class Node>
inline Node* BinaryTree<Node>::succ(Node* k, int delta)
{
	while ((delta > 0) && (k)) {
		k = succ(k);
		--delta;
	}
	return k;
}

template<class Node>
void BinaryTree<Node>::attachBefore(Node* kb, Node* kn)
{
	if (!kb)
		attach(kb, kn, true);
	else if (kb->left_)
		attach(max(kb->left_), kn, false);
	else
		attach(kb, kn, true);
}

template<class Node>
void BinaryTree<Node>::attachAfter(Node* ka, Node* kn)
{
	if (!ka)
		attach(ka, kn, true);
	else if (ka->right_)
		attach(min(ka->right_), kn, true);
	else
		attach(ka, kn, false);
}

template<class Node>
Node* BinaryTree<Node>::clone(Node* k)
{
	if (!k) return 0;
	Node* kn = new Node(*k);
	if (!k->parent_) kn->parent_ = 0;
	if (k->left_) {
		kn->left_ = clone(k->left_);
		kn->left_->parent_ = kn;
	}
	else {
		kn->left_ = 0;
	}
	if (k->right_) {
		kn->right_ = clone(k->right_);
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
void BinaryTree<Node>::clear(Node* k)
{
	if (!k) return;
	clear(k->left_);
	clear(k->right_);
	delete k;
}

/** Remove the node k from the tree.
  */
template<class Node>
void BinaryTree<Node>::remove(Node* k)
{
	if (k->left_)
		replaceNode(k, detach(max(k->left_)));
	else if (k->right_)
		replaceNode(k, detach(min(k->right_)));
	else
		detach(k);
	delete k;
}

#ifndef NDEBUG

template<class Node>
bool BinaryTree<Node>::testStructure(Node* k)
{
	if (!k) return true;
	if (k->parent_) {
		if (!((k == k->parent_->left_) || (k == k->parent_->right_)))
			return false;
	}
	return testStructure(k->left_) && testStructure(k->right_);
}

template<class Node>
bool BinaryTree<Node>::testOrder(Node* k)
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
}

template<class Node>
bool BinaryTree<Node>::testIteration(Node* k)
{
	if (k == 0) return true;
	Node* k2;
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

#endif // ndef NDEBUG

} // namespace ftl

#endif // FTL_BINARYTREE_HPP
