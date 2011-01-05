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
#include "BinaryNode.hpp"

namespace ftl
{

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

template<class T>
class BinaryTree: public NonCopyable
{
public:
	typedef BinaryNode<T> Node;
	
	BinaryTree(): root(0), _n(0) {}
	virtual ~BinaryTree() { clear(); }
	
	inline void clear() { clear(root); root = 0; _n = 0; }
	inline Node* rootNode() { return root; }
	
	inline int n() const { return _n; }
	inline int count() const { return _n; }
	
	inline Node* first() const { return (root != 0) ? root->min() : 0; }
	inline Node* last() const { return (root != 0) ? root->max() : 0; }
	template<class ST> inline Node* first(const ST& a) const;
	template<class ST> inline Node* last(const ST& b) const;
	
	inline T min() const { Node* k = first(); return (k != 0) ? k->data() : T(); }
	inline T max() const { Node* k = last(); return (k != 0) ? k->data() : T(); }
	
	template<class ST> Node* find(const ST& e, bool* found = 0) const;
	
	Node* unlink(Node* k);
	void clear(Node* k);
	
	void spliceIn(Node* kp, Node* kn);
	Node* spliceOut(Node* k);
	void replaceNode(Node* k0, Node* k1);
	
	void rotateLeft(Node* k);
	void rotateRight(Node* k);
	int height(Node* k);
	inline int height() { return height(root); }
	
	virtual void rebalanceAfterSpliceIn(Node* k, int delta) = 0;
	virtual void rebalanceAfterSpliceOut(Node* k, int delta) = 0;
	
	bool ok1(Node* k);  // condition: k->left->data() < k->right->data()
	bool ok2(Node* k);  // condition: (k == k->parent()->left) || (k == k->parent()->right)
	bool ok3(Node* k);  // condition: (k == k->parent()->left) || (k == k->parent()->right)
	inline bool ok() { return ok1(root) && ok2(root) && ok3(root); }
	
	inline Node* minNode() { return root->min(); }
	inline Node* maxNode() { return root->max(); }
	
	/*void levelPrint();
	void levelPrint(Node* k, int level);*/
	
protected:
	inline static int max(int a, int b) { return (a < b) ? b : a; }

	Node* root;
	int _n;
};

template<class T>
template<class ST>
typename BinaryTree<T>::Node* BinaryTree<T>::first(const ST& a) const
{
	bool found;
	Node* k = find(a, &found);
	if ((k == 0) || found) return k;
	if (a < k->data()) return k;
	return k->succ();
}

template<class T>
template<class ST>
typename BinaryTree<T>::Node* BinaryTree<T>::last(const ST& b) const
{
	bool found;
	Node* k = find(b, &found);
	if ((k == 0) || found) return k;
	if (k->data() < b) return k;
	return k->pred();
}
 
template<class T>
template<class ST>
typename BinaryTree<T>::Node* BinaryTree<T>::find(const ST& e, bool* found) const
{
	bool _found = false;
	Node* k = root;
	Node* k2 = 0;
	while ((k != 0) && (!_found))
	{
		k2 = k;
		if (e < k->data()) k = k->left;
		else if (k->data() < e) k = k->right;
		else { _found = true; break; }
	}
	if (found != 0) *found = _found;
	return k2;
}

template<class T>
typename BinaryTree<T>::Node* BinaryTree<T>::unlink(Node* k)
{
	if (k->left != 0)
		replaceNode(k, spliceOut(k->left->max()));
	else if (k->right != 0)
		replaceNode(k, spliceOut(k->right->min()));
	else
		spliceOut(k);
	return k;
}

template<class T>
void BinaryTree<T>::clear(Node* k)
{
	if (k == 0) return;
	clear(k->left);
	clear(k->right);
	delete k;
}

template<class T>
void BinaryTree<T>::spliceIn(Node* kp, Node* kn)
{
	++_n;
	if (kp == 0)
	{
		// -- splice in an initial node
		root = kn;
		kn->parent = 0;
		kn->left = 0;
		kn->right = 0;
	}
	else
	{
		// -- splice in a new leaf node
		if (kn->data() < kp->data())
			kp->left = kn;
		else
			kp->right =  kn;
		kn->parent = kp;
		kn->left = 0;
		kn->right = 0;
		rebalanceAfterSpliceIn(kp, 2 * (kp->left == kn) - 1);
	}
}

template<class T>
typename BinaryTree<T>::Node* BinaryTree<T>::spliceOut(Node* k)
{
	--_n;
	Node* kp = k->parent;
	int delta = 0;

	if (kp != 0)
		delta = 2 * (kp->left == k) - 1;

	// -- remove the parent's downlink
	//    or establish links between left or right child and parent
	if (kp != 0)
	{
		Node* kc = (k->left != 0) ? k->left : k->right;
		if (kp->left == k)
			kp->left = kc;
		else
			kp->right = kc;
		if (kc != 0) kc->parent = kp;
	}
	else
		root = 0;

	if (kp != 0)
		rebalanceAfterSpliceOut(kp, delta);

	return k;
}

template<class T>
void BinaryTree<T>::replaceNode(Node* k0, Node* k1)
{
	k1->misc = k0->misc;
	
	// -- establish links to the neighbors
	k1->parent = k0->parent;
	k1->left = k0->left;
	k1->right = k0->right;
	
	// -- establish links from neighbors
	Node* kp = k0->parent;
	if (kp != 0)
	{
		if (kp->left == k0)
			kp->left = k1;
		else
			kp->right = k1;
	}
	else
		root = k1;
	
	if (k0->left != 0)
		k0->left->parent = k1;
	
	if (k0->right != 0)
		k0->right->parent = k1;
}

template<class T>
void BinaryTree<T>::rotateLeft(Node* k1)
{
	Node* k2 = k1->right;

	if (k1->parent != 0)
	{
		if (k1->parent->left == k1)
			k1->parent->left = k2;
		else
			k1->parent->right = k2;
	}
	else
		root = k2;

	k2->parent = k1->parent;

	k1->parent = k2;
	k1->right = k2->left;

	if (k2->left != 0)
		k2->left->parent = k1;

	k2->left = k1;
}

template<class T>
void BinaryTree<T>::rotateRight(Node* k1)
{
	Node* k2 = k1->left;

	if (k1->parent != 0)
	{
		if (k1->parent->left == k1)
			k1->parent->left = k2;
		else
			k1->parent->right = k2;
	}
	else
		root = k2;

	k2->parent = k1->parent;

	k1->parent = k2;
	k1->left = k2->right;

	if (k2->right != 0)
		k2->right->parent = k1;

	k2->right = k1;
}

template<class T>
int BinaryTree<T>::height(Node* k)
{
	if (k == 0) return 0;
	return max(height(k->left), height(k->right)) + 1;
}

// == self test methods

template<class T>
bool BinaryTree<T>::ok1(Node* k)
{
	if (k == 0) return true;

	if (k->left != 0)
		if (!(k->left->data() < k->data())) return false;

	if (k->right != 0)
		if (!(k->data() < k->right->data())) return false;

	return ok1(k->left) && ok1(k->right);
}

template<class T>
bool BinaryTree<T>::ok2(Node* k)
{
	if (k == 0) return true;

	if (k->parent != 0)
		if (!((k == k->parent->left)||(k == k->parent->right))) return false;

	if (k->parent == 0)
		if (k != root) return false;

	return ok2(k->left) && ok2(k->right);
}

template<class T>
bool BinaryTree<T>::ok3(Node* k)
{
	if (k == 0) return true;

	Node* k2;

	k2 = k->succ();
	if (k2 != 0)
		if (k != k2->pred()) return false;

	k2 = k->pred();
	if (k2 != 0)
		if (k != k2->succ()) return false;

	return ok3(k->left) && ok3(k->right);
}

/*
template<class T>
void BinaryTree<T>::levelPrint()
{
	printf("level order print:\n");
	//fprintf(stderr, "n = %d\n", _n);
	for (int i = 0; i < height(root); ++i)
	{
		printf("%d: ", i);
		levelPrint(root, i);
		printf("\n");
	}
}

template<class T>
void BinaryTree<T>::levelPrint(Node* k, int level)
{
	if (k == 0) return;
	if (level == 0)
		printf("'%c' ", (char)k->data().value());
	else
	{
		levelPrint(k->left, level-1);
		levelPrint(k->right, level-1);
	}
}
*/

} // namespace ftl

#endif // FTL_BINARYTREE_HPP
