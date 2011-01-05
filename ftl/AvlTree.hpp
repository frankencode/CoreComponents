/*
 * AvlTree.hpp -- binary tree balancing logic
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

template<class T>
class AvlTree: public BinaryTree<T>
{
public:
	typedef typename BinaryTree<T>::Node Node;

	bool ok1(Node* k);
	bool ok2(Node* k);
	bool ok();

protected:
	virtual void rebalanceAfterSpliceIn(Node* k, int delta);
	virtual void rebalanceAfterSpliceOut(Node* k, int delta);
	Node* restoreBalance(Node* k1);
};

template<class T>
void AvlTree<T>::rebalanceAfterSpliceIn(Node* k, int delta)
{
	while (true)
	{
		k->balance += delta;
		
		if ((k->balance == 2) || (k->balance == -2))
			k = restoreBalance(k);
		
		if ((k->balance == 0) || (!k->parent))
			break;
		
		delta = 2 * (k->parent->left == k) - 1;
		k = k->parent;
	}
}

template<class T>
void AvlTree<T>::rebalanceAfterSpliceOut(Node* k, int delta)
{
	while (true)
	{
		k->balance -= delta;
		
		if ((k->balance == 2) || (k->balance == -2))
			k = restoreBalance(k);
		
		if ((k->balance != 0) || (!k->parent))
			break;
		
		delta = 2 * (k->parent->left == k) - 1;
		k = k->parent;
	}
}

template<class T>
typename AvlTree<T>::Node* AvlTree<T>::restoreBalance(Node* k1)
{
	if (k1->balance == 2)
	{
		Node* k2 = k1->left;

		if (k2->balance == -1)
		{
			// -- case Ia->III
			Node* k3 = k2->right;

			k1->balance = -(k3->balance == 1);
			k2->balance = (k3->balance == -1);
			k3->balance = 0;

			rotateLeft(k2);
			rotateRight(k1);
		}
		else
		{
			// -- case Ib, Ic
			k1->balance = (k2->balance <= 0);
			k2->balance = k2->balance - 1;

			rotateRight(k1);
		}
	}
	else
	{
		Node* k2 = k1->right;

		if (k2->balance == 1)
		{
			// -- case IIc->IV
			Node* k3 = k2->left;
			
			k1->balance = (k3->balance == -1);
			k2->balance = -(k3->balance == 1);
			k3->balance = 0;

			rotateRight(k2);
			rotateLeft(k1);
		}
		else
		{
			// -- case IIa, IIb
			k1->balance = -(k2->balance >= 0);
			k2->balance = k2->balance + 1;

			rotateLeft(k1);
		}
	}

	return k1->parent;
}

template<class T>
bool AvlTree<T>::ok1(Node* k)
{
	if (k == 0) return true;
	if (!((k->balance == -1) || (k->balance == 0) || (k->balance == 1))) return false;
	return ok1(k->left) && ok1(k->right);
}

template<class T>
bool AvlTree<T>::ok2(Node* k)
{
	if (k == 0) return true;
	if ((height(k->left) - height(k->right)) != k->balance) return false;
	return ok2(k->left) && ok2(k->right);
}

template<class T>
bool AvlTree<T>::ok()
{
	return BinaryTree<T>::ok() && ok1(BinaryTree<T>::root) && ok2(BinaryTree<T>::root);
}

} // namespace ftl

#endif // FTL_AVLTREE_HPP
