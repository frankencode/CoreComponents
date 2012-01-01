/*
 * OrdinalTree.hpp -- random access tree
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_ORDINALTREE_HPP
#define FTL_ORDINALTREE_HPP

#include "AvlTree.hpp"

namespace ftl
{

template<class ItemType>
class OrdinalNode
{
public:
	typedef ItemType Item;
	
	OrdinalNode(const Item& item)
		: item_(item),
		  balance_(0),
		  weight_(1)
	{}
	OrdinalNode(const OrdinalNode& b)
		: item_(b.item_),
		  balance_(b.balance_),
		  weight_(b.weight_)
	{}
	
	OrdinalNode* left_;
	OrdinalNode* right_;
	OrdinalNode* parent_;
	Item item_;
	int balance_;
	int weight_;
};

template<class NodeType>
class OrdinalTree: public AvlTree<NodeType>
{
public:
	typedef NodeType Node;
	typedef typename NodeType::Item Item;
	
	OrdinalTree();
	
	OrdinalTree(const OrdinalTree& b);
	const OrdinalTree& operator=(const OrdinalTree& b);
	
	inline int weight() const { return weight(BinaryTree<Node>::root_); }
	
	bool lookupByIndex(int index, Node** node = 0) const;
	
	template<class Pattern>
	Node* find(const Pattern& pattern, bool* found = 0, bool* below = 0, int* index = 0) const;
	
	int first(const Item& a) const;
	int last(const Item& b) const;
	
	void push(int index, const Item& item);
	void pop(int index, Item* item);
	
protected:
	void touched(Node* kp, Node* kc, bool left, bool attached);
	void rotated(Node* k1, bool left);
	void cleared();
	static void establishWeight(Node* k);
	inline static int weight(Node* k) { return (k) ? k->weight_ : 0; }
	
#ifndef NDEBUG
	static bool testWeight(Node* k);
#endif
	
	mutable Node* cachedNode_;
	mutable int cachedIndex_;
};

template<class Node>
OrdinalTree<Node>::OrdinalTree()
	: cachedNode_(0)
{}

template<class Node>
OrdinalTree<Node>::OrdinalTree(const OrdinalTree& b)
	: cachedNode_(0)
{
	BinaryTree<Node>::root_ = BinaryTree<Node>::clone(b.root_);
}

template<class Node>
const OrdinalTree<Node>& OrdinalTree<Node>::operator=(const OrdinalTree& b)
{
	BinaryTree<Node>::clear();
	BinaryTree<Node>::root_ = BinaryTree<Node>::clone(b.root_);
	return *this;
}

template<class Node>
bool OrdinalTree<Node>::lookupByIndex(int i, Node** node) const
{
	if (i < 0) i += weight();
	FTL_ASSERT((0 <= i) && (i < weight()));
	
	if (cachedNode_) {
		int d = i - cachedIndex_;
		if (d == 0) {
			if (node) *node = cachedNode_;
			return cachedNode_;
		}
		else if (d == 1) {
			++cachedIndex_;
			cachedNode_ = succ(cachedNode_);
			if ((cachedNode_) && (node)) *node = cachedNode_;
			return cachedNode_;
		}
		else if (d == -1) {
			--cachedIndex_;
			cachedNode_ = pred(cachedNode_);
			if ((cachedNode_) && (node)) *node = cachedNode_;
			return cachedNode_;
		}
	}
	
	Node* k = BinaryTree<Node>::root_;
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
	
	cachedNode_ = k;
	cachedIndex_ = i;
	
	return k;
}

template<class Node>
template<class Pattern>
Node* OrdinalTree<Node>::find(const Pattern& pattern, bool* found, bool* below, int* index) const
{
	Node* k = BinaryTree<Node>::root_;
	Node* k2 = 0;
	if (found) *found = false;
	int j0 = 0, j = -1;
	if (k) while (true) {
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
	if (index) *index = j;
	return k2;
}

template<class Node>
inline int OrdinalTree<Node>::first(const Item& a) const
{
	bool found = false, below = true;
	int index = 0;
	if (!BinaryTree<Node>::root_) return 0;
	find(a, &found, &below, &index);
	if (found) return index;
	return below ? index : index + 1;
}

template<class Node>
inline int OrdinalTree<Node>::last(const Item& b) const
{
	bool found = false, below = true;
	int index = 0;
	if (!BinaryTree<Node>::root_) return 0;
	find(b, &found, &below, &index);
	if (found) return index;
	return below ? index - 1 : index;
}

template<class Node>
void OrdinalTree<Node>::push(int index, const Item& item)
{
	Node* kn = new Node(item);
	if (index == weight()) {
		Node* kp = 0;
		if (cachedNode_)
			if (cachedIndex_ == index)
				kp = cachedNode_;
		if (!kp) kp = BinaryTree<Node>::max();
		attach(kp, kn, false);
	}
	else {
		Node* ka = 0;
		bool found = lookupByIndex(index, &ka);
		FTL_ASSERT(found);
		attachBefore(ka, kn);
	}
	cachedNode_ = kn;
	cachedIndex_ = index;
}

template<class Node>
void OrdinalTree<Node>::pop(int index, Item* item)
{
	Node* ko = 0;
	bool found = lookupByIndex(index, &ko);
	FTL_ASSERT(found);
	*item = ko->item_;
	Node* k = pred(ko);
	if (k) --index;
	else k = succ(ko);
	BinaryTree<Node>::remove(ko);
	if (k) {
		cachedNode_ = k;
		cachedIndex_ = index;
	}
}

template<class Node>
inline void OrdinalTree<Node>::touched(Node* kp, Node* kc, bool left, bool attached)
{
	int delta = attached ? 1 : -1;
	Node* k = kp;
	while (k) {
		k->weight_ += delta;
		k = k->parent_;
	}
	
	AvlTree<Node>::touched(kp, kc, left, attached);
	
	/*FTL_ASSERT(BinaryTree<Node>::testStructure(BinaryTree<Node>::root_));
	FTL_ASSERT(BinaryTree<Node>::testIteration(BinaryTree<Node>::root_));
	FTL_ASSERT(AvlTree<Node>::testBalance(BinaryTree<Node>::root_));
	FTL_ASSERT(OrdinalTree<Node>::testWeight(BinaryTree<Node>::root_));*/
	
	cachedNode_ = 0;
}

template<class Node>
inline void OrdinalTree<Node>::rotated(Node* k1, bool left)
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
inline void OrdinalTree<Node>::establishWeight(Node* k)
{
	k->weight_ = weight(k->left_) + weight(k->right_) + 1;
}

#ifndef NDEBUG
template<class Node>
bool OrdinalTree<Node>::testWeight(Node* k)
{
	if (!k) return true;
	return
		(weight(k->left_) + weight(k->right_) + 1 == k->weight_) &&
		testWeight(k->left_) && testWeight(k->right_);
}
#endif // ndef NDEBUG

} // namespace ftl

#endif // FTL_ORDINALTREE_HPP
