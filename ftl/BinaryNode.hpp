/*
 * BinaryTree.hpp -- binary tree node
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_BINARYNODE_HPP
#define FTL_BINARYNODE_HPP

namespace ftl
{

template<class T>
class BinaryNode
{
public:
	typedef BinaryNode Node;
	
	BinaryNode(const T& e): misc(0), e_(e) {}
	
	Node* left;
	Node* right;
	Node* parent;

	union {
		int misc;
		int balance;
		int color;
	};
	
	inline const T& data() const { return e_; }
	inline const T& operator*() const { return e_; }
	inline const T* dataPtr() const { return &e_; }
	
	inline Node* previous() { return pred(); }
	inline Node* next() { return succ(); }
	inline Node* previous(int delta) { return pred(delta); }
	inline Node* next(int delta) { return succ(delta); }
	
	inline Node* min()
	{
		Node* k = this;
		Node* k2 = k;
		while (k) {
			k2 = k;
			k = k->left;
		}
		return k2;
	}
	
	inline Node* max()
	{
		Node* k = this;
		Node* k2 = k;
		while (k) {
			k2 = k;
			k = k->right;
		}
		return k2;
	}
	
	Node* pred()
	{
		Node* k = this;
		
		if (k->left != 0)
			return k->left->max();
	
		Node* kp = k->parent;
	
		while (kp != 0)
		{
			if (k == kp->right) break;
			k = kp;
			kp = kp->parent;
		}
	
		return kp;
	}
	
	Node* succ()
	{
		Node* k = this;
		
		if (k->right != 0)
			return k->right->min();
	
		Node* kp = k->parent;
	
		while (kp != 0)
		{
			if (k == kp->left) break;
			k = kp;
			kp = kp->parent;
		}
	
		return kp;
	}
	
	inline Node* pred(int delta)
	{
		Node* k = this;
		while ((delta > 0) && (k != 0)) {
			k = k->pred();
			--delta;
		}
		return k;
	}
	
	inline Node* succ(int delta)
	{
		Node* k = this;
		while ((delta > 0) && (k != 0)) {
			k = k->succ();
			--delta;
		}
		return k;
	}
	
	T e_;
};

} // namespace ftl

#endif // FTL_BINARYNODE_HPP
