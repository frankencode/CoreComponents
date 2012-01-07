/*
 * LayoutTree.hpp -- layout tree
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_LAYOUTTREE_HPP
#define FTL_LAYOUTTREE_HPP

#include "Property.hpp"
#include "OrdinalTree.hpp"

namespace ftl
{

template<class ItemType>
class LayoutNode
{
public:
	typedef ItemType Item;
	typedef typename ItemType::Extend Extend;
	
	LayoutNode(const Item& item)
		: item_(item),
		  balance_(0),
		  weight_(1),
		  extend_(item.extend)
	{
		item_.extend.valueChanged()->connect(this, &LayoutNode::itemExtendChanged);
	}
	LayoutNode(const LayoutNode& b)
		: item_(b.item_),
		  balance_(b.balance_),
		  weight_(b.weight_),
		  extend_(b.extend_)
	{
		item_.extend.valueChanged()->connect(this, &LayoutNode::itemExtendChanged);
	}
	
	inline void establishExtend()
	{
		extend_ = item_.extend
			+ ((left_) ? left_->extend_ : Extend())
			+ ((right_) ? right_->extend_ : Extend());
	}
	
	void itemExtendChanged(Extend)
	{
		LayoutNode* k = this;
		while (k) {
			k->establishExtend();
			k = k->parent_;
		}
	}
	
	LayoutNode* left_;
	LayoutNode* right_;
	LayoutNode* parent_;
	Item item_;
	int balance_;
	int weight_;
	Extend extend_;
};

template<class NodeType>
class LayoutTree: public OrdinalTree<NodeType>
{
public:
	typedef NodeType Node;
	typedef typename NodeType::Item Item;
	typedef typename NodeType::Extend Extend;
	
	inline Extend extend() const { return extend(BinaryTree<Node>::root_); }
	
	int find(Extend x) const;
	int first(Extend a) const;
	int last(Extend b) const;
	
protected:
	void touched(Node* kp, Node* kc, bool left, bool attached);
	void rotated(Node* k1, bool left);
	inline static void establishExtend(Node* k) { k->establishExtend(); }
	inline static Extend extend(Node* k) { return (k) ? k->extend_ : Extend(); }
};

template<class Node>
int LayoutTree<Node>::find(Extend x) const
{
	Node* k = BinaryTree<Node>::root_;
	int j0 = 0, j = -1;
	Extend y0 = 0;
	if (k) {
		while (true) {
			j = j0 + weight(k->left_);
			Extend x0 = y0 + extend(k->left_);
			Extend x1 = x0 + k->item_.extend;
			if (x < x0) {
				if (!k->left_) break;
				k = k->left_;
			}
			else if (x < x1) {
				break;
			}
			else {
				if (!k->right_) break;
				j0 = j + 1;
				y0 = x1;
				k = k->right_;
			}
		}
	}
	return j;
}

template<class Node>
inline int LayoutTree<Node>::first(Extend a) const
{
	if (OrdinalTree<Node>::weight() == 0) return -1;
	if (a < 0) return 0;
	if (a >= extend()) return -1;
	return find(a);
}

template<class Node>
inline int LayoutTree<Node>::last(Extend b) const
{
	if (OrdinalTree<Node>::weight() == 0) return -1;
	if (b <= 0) return -1;
	if (b >= extend()) return OrdinalTree<Node>::weight() - 1;
	return find(b);
}

template<class Node>
void LayoutTree<Node>::touched(Node* kp, Node* kc, bool left, bool attached)
{
	OrdinalTree<Node>::touched(kp, kc, left, attached);
	
	Extend delta = kc->extend_;
	if (!attached) delta = -delta;
	Node* k = kp;
	while (k) {
		k->extend_ += delta;
		k = k->parent_;
	}
}

template<class Node>
void LayoutTree<Node>::rotated(Node* k1, bool left)
{
	OrdinalTree<Node>::rotated(k1, left);
	
	establishExtend(k1);
	establishExtend(k1->parent_);
}

} // namespace ftl

#endif // FTL_LAYOUTTREE_HPP
