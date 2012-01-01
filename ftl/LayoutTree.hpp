/*
 * LayoutTree.hpp -- layout tree
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_LAYOUTTREE_HPP
#define FTL_LAYOUTTREE_HPP

#include "OrdinalTree.hpp"

namespace ftl
{

template<class Item>
class DefaultExtendPolicy {
public:
	typedef double Extend;
	inline static double extend(const Item& item) { return item; }
};

template<class ItemType, class ExtendPolicyType = DefaultExtendPolicy<ItemType> >
class LayoutNode
{
public:
	typedef ItemType Item;
	typedef ExtendPolicyType ExtendPolicy;
	typedef typename ExtendPolicy::Extend Extend;
	
	LayoutNode(const Item& item)
		: item_(item),
		  balance_(0),
		  weight_(1),
		  extend_(ExtendPolicy::extend(item))
	{}
	LayoutNode(const LayoutNode& b)
		: item_(b.item_),
		  balance_(b.balance_),
		  weight_(b.weight_),
		  extend_(b.extend_)
	{}
	
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
	typedef typename NodeType::ExtendPolicy ExtendPolicy;
	typedef typename ExtendPolicy::Extend Extend;
	
protected:
	void touched(Node* kp, Node* kc, bool left, bool attached);
	void rotated(Node* k1, bool left);
	static void establishExtend(Node* k);
	inline static Extend extend(Node* k) { return (k) ? k->extend_ : Extend(); }
};

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

template<class Node>
inline void LayoutTree<Node>::establishExtend(Node* k)
{
	k->extend_ = extend(k->left_) + extend(k->right_) + ExtendPolicy::extend(k->item_);
}

} // namespace ftl

#endif // FTL_LAYOUTTREE_HPP
