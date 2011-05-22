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

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

template<class T>
class BinaryNode
{
public:
	typedef BinaryNode Node;
	
	BinaryNode(const T& e)
		: balance(0),
		  weight(1),
		  e_(e)
	{}
	
	Node* left;
	Node* right;
	Node* parent;
	int balance;
	int weight;
	T e_;
	
	inline const T& data() const { return e_; }
	
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
		if (left)
			return left->max();
	
		Node* kp = parent;
		Node* k = this;
		while (kp) {
			if (k == kp->right) break;
			k = kp;
			kp = kp->parent;
		}
	
		return kp;
	}
	
	Node* succ()
	{
		if (right)
			return right->min();
	
		Node* kp = parent;
		Node* k = this;
		while (kp) {
			if (k == kp->left) break;
			k = kp;
			kp = kp->parent;
		}
	
		return kp;
	}
	
	inline Node* pred(int delta)
	{
		Node* k = this;
		while ((delta > 0) && (k)) {
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
};

template<class T>
class BinaryTree: public NonCopyable
{
public:
	typedef BinaryNode<T> Node;
	
	BinaryTree(): root(0), cachedNode(0), cachedIndex(-1) {}
	virtual ~BinaryTree() { clear(); }
	
	template<class ST> inline Node* first(const ST& a) const;
	template<class ST> inline Node* last(const ST& b) const;
	inline Node* minNode() { return (root) ? root->min() : 0; }
	inline Node* maxNode() { return (root) ? root->max() : 0; }
	
	inline int weight() const { return weight(root); }
	
	bool lookupByIndex(int index, Node** node = 0) const;
	template<class ST> Node* find(const ST& e, bool* found = 0, bool* below = 0, int* index = 0) const;
	
	Node* unlink(Node* k);
	void clear() { clear(root); root = 0; }
	static void clear(Node* k);
	
	void push(int index, const T& item);
	void pop(int index, T* item);
	
	void spliceInBefore(Node* kb, Node* kn);
	void spliceInAfter(Node* ka, Node* kn);
	
	void spliceIn(Node* kp, Node* kn, bool left);
	Node* spliceOut(Node* k);
	
	inline bool health() {
		// levelPrint();
		/*check(ok1(root));*/
		check(ok2(root));
		check(ok3(root));
		check(testBalance1(root));
		check(testBalance2(root));
		check(testWeight(root));
		return /*ok1(root) &&*/ ok2(root) && ok3(root) && testBalance1(root) && testBalance2(root) && testWeight(root);
	}
	
protected:
	void replaceNode(Node* k0, Node* k1);
	
	void rotateLeft(Node* k);
	void rotateRight(Node* k);
	void rebalanceAfterSpliceIn(Node* kp, Node* kn);
	void rebalanceBeforeSpliceOut(Node* kp, Node* ko);
	Node* restoreBalance(Node* k1);
	
	static void restoreWeights(Node* k, int delta);
	inline static void updateWeight(Node* k) { k->weight = weight(k->left) + weight(k->right) + 1; }
	inline static int weight(Node* k) { return (k) ? k->weight : 0; }
	
	static bool ok1(Node* k);  // condition: k->left->data() < k->right->data()
	static bool ok2(Node* k);  // condition: (k == k->parent()->left) || (k == k->parent()->right)
	static bool ok3(Node* k);  // condition: (k == k->parent()->left) || (k == k->parent()->right)
	
	static int height(Node* k);
	inline int height() { return height(root); }
	static bool testBalance1(Node* k);
	static bool testBalance2(Node* k);
	static bool testWeight(Node* k);
	
	void levelPrint();
	void levelPrint(Node* k, int level);
	
	inline static int max(int a, int b) { return (a < b) ? b : a; }
	
	Node* root;
	mutable Node* cachedNode;
	mutable int cachedIndex;
};

template<class T>
template<class ST>
typename BinaryTree<T>::Node* BinaryTree<T>::first(const ST& a) const
{
	bool found = false, below = true;
	Node* k = find(a, &found, &below);
	if ((k == 0) || found) return k;
	return below ? k->succ() : k;
}

template<class T>
template<class ST>
typename BinaryTree<T>::Node* BinaryTree<T>::last(const ST& b) const
{
	bool found = false, below = true;
	Node* k = find(b, &found, &below);
	if ((k == 0) || found) return k;
	return below ? k : k->pred();
}

template<class T>
bool BinaryTree<T>::lookupByIndex(int i, Node** node) const
{
	if (i < 0) i += weight(root);
	check((0 <= i) && (i < weight(root)));
	
	if (cachedNode) {
		int d = i - cachedIndex;
		if (d == 0) {
			if (node) *node = cachedNode;
			return cachedNode;
		}
		else if (d == 1) {
			++cachedIndex;
			cachedNode = cachedNode->succ();
			if ((cachedNode) && (node)) *node = cachedNode;
			return cachedNode;
		}
		else if (d == -1) {
			--cachedIndex;
			cachedNode = cachedNode->pred();
			if ((cachedNode) && (node)) *node = cachedNode;
			return cachedNode;
		}
	}
	
	Node* k = root;
	int j0 = 0;
	while (k) {
		int j = j0 + weight(k->left);
		if (i < j) {
			k = k->left;
		}
		else if (j < i) {
			j0 = j + 1;
			k = k->right;
		}
		else // i == j
			break;
	}
	if ((k) && (node)) *node = k;
	
	cachedNode = k;
	cachedIndex = i;
	
	return k;
}

template<class T>
template<class ST>
typename BinaryTree<T>::Node* BinaryTree<T>::find(const ST& e, bool* found, bool* below, int* index) const
{
	Node* k = root;
	Node* k2 = 0;
	if (found) *found = false;
	int j0 = 0, j = -1;
	if (k) while (true) {
		k2 = k;
		j = j0 + weight(k->left);
		if (e < k->data()) {
			if (!k->left) {
				if (below) *below = true;
				break;
			}
			k = k->left;
		}
		else if (k->data() < e) {
			if (!k->right) {
				if (below) *below = false;
				break;
			}
			j0 = j + 1;
			k = k->right;
		}
		else { // e == k->data()
			if (found) *found = true;
			break;
		}
	}
	if (index) *index = j;
	return k2;
}

template<class T>
typename BinaryTree<T>::Node* BinaryTree<T>::unlink(Node* k)
{
	if (k->left)
		replaceNode(k, spliceOut(k->left->max()));
	else if (k->right)
		replaceNode(k, spliceOut(k->right->min()));
	else
		spliceOut(k);
	return k;
}

template<class T>
void BinaryTree<T>::clear(Node* k)
{
	if (!k) return;
	clear(k->left);
	clear(k->right);
	delete k;
}

template<class T>
void BinaryTree<T>::push(int index, const T& item)
{
	Node* kn = new Node(item);
	if (index == weight(root)) {
		Node* kp = 0;
		if (cachedNode)
			if (cachedIndex == index)
				kp = cachedNode;
		if (!kp) kp = maxNode();
		spliceIn(kp, kn, false/*left*/);
	}
	else {
		Node* ka = 0;
		bool found = lookupByIndex(index, &ka);
		check(found);
		spliceInBefore(ka, kn);
	}
	cachedNode = kn;
	cachedIndex = index;
}

template<class T>
void BinaryTree<T>::pop(int index, T* item)
{
	Node* ko = 0;
	bool found = lookupByIndex(index, &ko);
	check(found);
	*item = ko->e_;
	Node* k = ko->pred();
	if (k) --index;
	else k = ko->succ();
	delete unlink(ko);
	if (k) {
		cachedNode = k;
		cachedIndex = index;
	}
}

template<class T>
void BinaryTree<T>::spliceInBefore(Node* kb, Node* kn)
{
	if (!kb)
		spliceIn(kb, kn, true/*left*/);
	else if (kb->left)
		spliceIn(kb->left->max(), kn, false/*left*/);
	else
		spliceIn(kb, kn, true/*left*/);
}

template<class T>
void BinaryTree<T>::spliceInAfter(Node* ka, Node* kn)
{
	if (!ka)
		spliceIn(ka, kn, true/*left*/);
	else if (ka->right)
		spliceIn(ka->right->min(), kn, true/*left*/);
	else
		spliceIn(ka, kn, false/*left*/);
}

template<class T>
void BinaryTree<T>::spliceIn(Node* kp, Node* kn, bool left)
{
	if (!kp) {
		// -- splice in initial node
		root = kn;
		kn->parent = 0;
		kn->left = 0;
		kn->right = 0;
	}
	else {
		// -- splice in new leaf node
		if (left)
			kp->left = kn;
		else
			kp->right = kn;
		kn->parent = kp;
		kn->left = 0;
		kn->right = 0;
		
		restoreWeights(kp, 1);
		rebalanceAfterSpliceIn(kp, kn);
	}
	
	cachedNode = 0;
}

template<class T>
typename BinaryTree<T>::Node* BinaryTree<T>::spliceOut(Node* k)
{
	Node* kp = k->parent;

	if (!kp) {
		// -- splice out the root
		root = 0;
	}
	else {
		rebalanceBeforeSpliceOut(kp, k);
		
		// -- splice out node
		Node* kc = (k->left != 0) ? k->left : k->right;
		if (kp->left == k)
			kp->left = kc;
		else
			kp->right = kc;
		if (kc != 0) kc->parent = kp;
		
		restoreWeights(kp, -1);
	}
	
	cachedNode = 0;
	
	return k;
}

/** Putting the leaf node kl into the place of inner node ki
  */
template<class T>
void BinaryTree<T>::replaceNode(Node* ki, Node* kl)
{
	kl->balance = ki->balance;
	kl->weight = ki->weight;
	
	// establish links to neighbors
	kl->parent = ki->parent;
	kl->left = ki->left;
	kl->right = ki->right;
	
	// establish links from neighbors
	Node* kp = ki->parent;
	if (kp) {
		if (kp->left == ki)
			kp->left = kl;
		else
			kp->right = kl;
	}
	else
		root = kl;
	if (ki->left)
		ki->left->parent = kl;
	if (ki->right)
		ki->right->parent = kl;
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
	
	updateWeight(k1);
	updateWeight(k1->parent);
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
	
	updateWeight(k1);
	updateWeight(k1->parent);
}

template<class T>
void BinaryTree<T>::rebalanceAfterSpliceIn(Node* kp, Node* kn)
{
	int delta = 2 * (kp->left == kn) - 1;
	Node* k = kp;
	
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
void BinaryTree<T>::rebalanceBeforeSpliceOut(Node* kp, Node* ko)
{
	int delta = 2 * (kp->left == ko) - 1;
	Node* k = kp;
	
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
typename BinaryTree<T>::Node* BinaryTree<T>::restoreBalance(Node* k1)
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
void BinaryTree<T>::restoreWeights(Node* k, int delta)
{
	while (k) {
		k->weight += delta;
		k = k->parent;
	}
}

template<class T>
int BinaryTree<T>::height(Node* k)
{
	if (k == 0) return 0;
	return max(height(k->left), height(k->right)) + 1;
}

template<class T>
bool BinaryTree<T>::ok1(Node* k)
{
	if (k == 0) return true;

	if (k->left)
		if (!(k->left->data() < k->data())) return false;

	if (k->right)
		if (!(k->data() < k->right->data())) return false;

	return ok1(k->left) && ok1(k->right);
}

template<class T>
bool BinaryTree<T>::ok2(Node* k)
{
	if (k == 0) return true;

	if (k->parent)
		if (!((k == k->parent->left)||(k == k->parent->right))) return false;

	return ok2(k->left) && ok2(k->right);
}

template<class T>
bool BinaryTree<T>::ok3(Node* k)
{
	if (k == 0) return true;

	Node* k2;

	k2 = k->succ();
	if (k2)
		if (k != k2->pred()) return false;

	k2 = k->pred();
	if (k2)
		if (k != k2->succ()) return false;

	return ok3(k->left) && ok3(k->right);
}

template<class T>
bool BinaryTree<T>::testBalance1(Node* k)
{
	if (!k) return true;
	if (!((k->balance == -1) || (k->balance == 0) || (k->balance == 1))) return false;
	return testBalance1(k->left) && testBalance1(k->right);
}

template<class T>
bool BinaryTree<T>::testBalance2(Node* k)
{
	if (!k) return true;
	check(((height(k->left) - height(k->right)) == k->balance));
	if ((height(k->left) - height(k->right)) != k->balance) return false;
	return testBalance2(k->left) && testBalance2(k->right);
}

template<class T>
bool BinaryTree<T>::testWeight(Node* k)
{
	if (!k) return true;
	return
		(weight(k->left) + weight(k->right) + 1 == k->weight) &&
		testWeight(k->left) && testWeight(k->right);
}

/*template<class T>
void BinaryTree<T>::levelPrint()
{
	printf("level order print:\n");
	for (int i = 0; i < height(root); ++i) {
		printf("%d: ", i);
		levelPrint(root, i);
		printf("\n");
	}
}

template<class T>
void BinaryTree<T>::levelPrint(Node* k, int level)
{
	if (k == 0) return;
	if (level == 0) {
		printf("(%p: %p,%p,%p,%d) ", k, k->parent, k->left, k->right, k->balance);
	}
	else {
		levelPrint(k->left, level-1);
		levelPrint(k->right, level-1);
	}
}*/

} // namespace ftl

#endif // FTL_BINARYTREE_HPP
