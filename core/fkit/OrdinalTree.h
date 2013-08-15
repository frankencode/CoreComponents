/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_ORDINALTREE_H
#define FKIT_ORDINALTREE_H

#include "AvlTree.h"
#include "Array.h"
#include "ExclusiveAccess.h"

namespace fkit
{

template<class ItemType>
class OrdinalNode
{
public:
	typedef ItemType Item;

	OrdinalNode()
		: weight_(1),
		  balance_(0),
		  item_(Item())
	{}
	OrdinalNode(const Item &item)
		: weight_(1),
		  balance_(0),
		  item_(item)
	{}
	OrdinalNode(const OrdinalNode &b)
		: weight_(b.weight_),
		  balance_(b.balance_),
		  item_(b.item_)
	{}

	OrdinalNode *left_;
	OrdinalNode *right_;
	OrdinalNode *parent_;
	int weight_;
	int balance_;
	Item item_;
};

class Index
{
public:
	Index(int i): i_(i), tree_(0) {}
	inline operator int() const { return i_; }
	inline Index &operator=(int b) { i_ = b; return *this; }

	inline Index &operator+=(int b) { i_ += b; return *this; }
	inline Index &operator-=(int b) { i_ -= b; return *this; }
	inline Index &operator*=(int b) { i_ *= b; return *this; }
	inline Index &operator/=(int b) { i_ /= b; return *this; }
	inline Index &operator++() { ++i_; return *this; }
	inline Index &operator--() { --i_; return *this; }
	inline Index operator++(int) { Index h = *this; ++i_; return h; }
	inline Index operator--(int) { Index h = *this; --i_; return h; }

private:
	template<class> friend class OrdinalTree;

	int i_, cachedIndex_;
	void *tree_, *cachedNode_;
	uint64_t revision_;
};

template<class NodeType>
class OrdinalTree: public AvlTree<NodeType>
{
public:
	typedef NodeType Node;
	typedef typename NodeType::Item Item;

	OrdinalTree();
	OrdinalTree(int n);

	OrdinalTree(const OrdinalTree &b);
	const OrdinalTree &operator=(const OrdinalTree &b);

	inline int weight() const { return weight(this->root_); }

	bool lookupByIndex(int index, Node **node = 0) const;

	template<class Pattern>
	Node *find(const Pattern &pattern, bool *found = 0, bool *below = 0, int *index = 0) const;

	int first(const Item &a) const;
	int last(const Item &b) const;

	void push(int index, const Item &item);
	void pop(int index, Item *item);

protected:
	void touched(Node *kp, Node *kc, bool left, bool attached);
	void rotated(Node *k1, bool left);
	void cleared();
	static void establishWeight(Node *k);
	inline static int weight(Node *k) { return (k) ? k->weight_ : 0; }

#ifndef NDEBUG
	static bool testWeight(Node *k);
#endif

	mutable ExclusiveSection cacheExclusive_;
	mutable Node *cachedNode_;
	mutable int cachedIndex_;
};

template<class Node>
OrdinalTree<Node>::OrdinalTree()
	: cachedNode_(0)
{}

template<class Node>
OrdinalTree<Node>::OrdinalTree(int n)
	: cachedNode_(0)
{
	if (n <= 0) return;

	Ref< Array<Node *> > v = Array<Node *>::create(n);

	for (int i = 0, m = 1; i < n; m *= 2)
	{
		for (int i1 = i + m; i < i1; ++i)
		{
			if (i < n) {
				Node *k = new Node;
				Node *kp = 0;
				if (i > 0) kp = v->at((i - 1) >> 1);
				k->parent_ = kp;
				if (kp) {
					if (i & 1)
						kp->left_ = k;
					else
						kp->right_ = k;
				}
				k->left_ = 0;
				k->right_ = 0;
				v->at(i) = k;
			}
			else {
				AvlTree<Node>::touched(v->at((i - 1) >> 1), 0, i & 1, false);
			}
		}
	}

	for (int i = n - 1; i > 0; --i) {
		Node *k = v->at(i);
		Node *kp = k->parent_;
		kp->weight_ += k->weight_;
	}

	this->root_ = v->at(0);

	/*FKIT_ASSERT(BinaryTree<Node>::testStructure(this->root_));
	FKIT_ASSERT(BinaryTree<Node>::testIteration(this->root_));
	FKIT_ASSERT(AvlTree<Node>::testBalance(this->:root_));
	FKIT_ASSERT(OrdinalTree<Node>::testWeight(this->root_));*/
}

template<class Node>
OrdinalTree<Node>::OrdinalTree(const OrdinalTree &b)
	: cachedNode_(0)
{
	this->root_ = BinaryTree<Node>::clone(b.root_);
}

template<class Node>
const OrdinalTree<Node> &OrdinalTree<Node>::operator=(const OrdinalTree &b)
{
	this->clear();
	this->root_ = BinaryTree<Node>::clone(b.root_);
	return *this;
}

template<class Node>
bool OrdinalTree<Node>::lookupByIndex(int i, Node **node) const
{
	FKIT_ASSERT((0 <= i) && (i < weight()));

	ExclusiveAccess cacheAccess(&cacheExclusive_);
	if (cacheAccess) {
		if (cachedNode_) {
			const int d = i - cachedIndex_;
			if (d == 1) {
				++cachedIndex_;
				cachedNode_ = this->succ(cachedNode_);
				if ((cachedNode_) && (node)) *node = cachedNode_;
				return cachedNode_;
			}
			else if (d == 0) {
				if (node) *node = cachedNode_;
				return cachedNode_;
			}
			else if (d == -1) {
				--cachedIndex_;
				cachedNode_ = this->pred(cachedNode_);
				if ((cachedNode_) && (node)) *node = cachedNode_;
				return cachedNode_;
			}
		}
	}

	Node *k = this->root_;
	int j0 = 0;
	while (k) {
		int j = j0 + weight(k->left_);
		if (i < j) {
			k = k->left_;
		}
		else if (j < i) {
			j0 = j + 1;
			k = k->right_;
		}
		else // i == j
			break;
	}
	if ((k) && (node)) *node = k;

	if (cacheAccess) {
		cachedNode_ = k;
		cachedIndex_ = i;
	}

	return k;
}

template<class Node>
template<class Pattern>
Node *OrdinalTree<Node>::find(const Pattern &pattern, bool *found, bool *below, int *index) const
{
	Node *k = this->root_;
	Node *k2 = 0;
	if (found) *found = false;
	int j0 = 0, j = -1;
	if (k) {
		while (true) {
			k2 = k;
			j = j0 + weight(k->left_);
			if (pattern < k->item_) {
				if (!k->left_) {
					if (below) *below = true;
					break;
				}
				k = k->left_;
			}
			else if (k->item_ < pattern) {
				if (!k->right_) {
					if (below) *below = false;
					break;
				}
				j0 = j + 1;
				k = k->right_;
			}
			else { // item == k->data()
				if (found) *found = true;
				break;
			}
		}
	}
	if (index) *index = j;
	return k2;
}

template<class Node>
inline int OrdinalTree<Node>::first(const Item &a) const
{
	bool found = false, below = true;
	int index = 0;
	if (!this->root_) return 0;
	find(a, &found, &below, &index);
	if (found) return index;
	return below ? index : index + 1;
}

template<class Node>
inline int OrdinalTree<Node>::last(const Item &b) const
{
	bool found = false, below = true;
	int index = 0;
	if (!this->root_) return 0;
	find(b, &found, &below, &index);
	if (found) return index;
	return below ? index - 1 : index;
}

template<class Node>
void OrdinalTree<Node>::push(int index, const Item &item)
{
	Node *kn = new Node(item);
	if (index == weight()) {
		Node *kp = 0;
		if (cachedNode_) {
			if (cachedIndex_ == index)
				kp = cachedNode_;
		}
		if (!kp) kp = this->max();
		this->attach(kp, kn, false);
	}
	else {
		Node *ka = 0;
		if (!lookupByIndex(index, &ka))
			FKIT_ASSERT(false);
		this->attachBefore(ka, kn);
	}

	cachedNode_ = kn;
	cachedIndex_ = index;
}

template<class Node>
void OrdinalTree<Node>::pop(int index, Item *item)
{
	Node *ko = 0;
	if (!lookupByIndex(index, &ko))
		FKIT_ASSERT(false);
	*item = ko->item_;
	Node *k = this->pred(ko);
	if (k) --index;
	else k = this->succ(ko);
	this->remove(ko);
	if (k) {
		cachedNode_ = k;
		cachedIndex_ = index;
	}
}

template<class Node>
inline void OrdinalTree<Node>::touched(Node *kp, Node *kc, bool left, bool attached)
{
	int delta = attached ? 1 : -1;
	Node *k = kp;
	while (k) {
		k->weight_ += delta;
		k = k->parent_;
	}

	AvlTree<Node>::touched(kp, kc, left, attached);

	cachedNode_ = 0;
}

template<class Node>
inline void OrdinalTree<Node>::rotated(Node *k1, bool left)
{
	establishWeight(k1);
	establishWeight(k1->parent_);
}

template<class Node>
void OrdinalTree<Node>::cleared()
{
	cachedNode_ = 0;
}

template<class Node>
inline void OrdinalTree<Node>::establishWeight(Node *k)
{
	k->weight_ = weight(k->left_) + weight(k->right_) + 1;
}

#ifndef NDEBUG
template<class Node>
bool OrdinalTree<Node>::testWeight(Node *k)
{
	if (!k) return true;
	return
		(weight(k->left_) + weight(k->right_) + 1 == k->weight_) &&
		testWeight(k->left_) && testWeight(k->right_);
}
#endif // ndef NDEBUG

} // namespace fkit

#endif // FKIT_ORDINALTREE_H
