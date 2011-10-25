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
	
	BinaryNode(const T& item, const int balance = 0, const int weight = 1)
		: item_(item),
		  balance_(balance),
		  weight_(weight)
	{}
	
	Node* left_;
	Node* right_;
	Node* parent_;
	T item_;
	int balance_;
	int weight_;
	
	// inline const T& data() const { return item_; }
	
	inline Node* min()
	{
		Node* k = this;
		Node* k2 = k;
		while (k) {
			k2 = k;
			k = k->left_;
		}
		return k2;
	}
	
	inline Node* max()
	{
		Node* k = this;
		Node* k2 = k;
		while (k) {
			k2 = k;
			k = k->right_;
		}
		return k2;
	}
	
	Node* pred()
	{
		if (left_)
			return left_->max();
	
		Node* kp = parent_;
		Node* k = this;
		while (kp) {
			if (k == kp->right_) break;
			k = kp;
			kp = kp->parent_;
		}
	
		return kp;
	}
	
	Node* succ()
	{
		if (right_)
			return right_->min();
	
		Node* kp = parent_;
		Node* k = this;
		while (kp) {
			if (k == kp->left_) break;
			k = kp;
			kp = kp->parent_;
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
	
	BinaryTree(const BinaryTree& b): root(clone(b.root)), cachedNode(0), cachedIndex(-1) {}
	const BinaryTree& operator=(const BinaryTree& b) {
		clear();
		root = clone(b.root);
		return *this;
	}
	
	void clear() {
		clear(root);
		root = 0;
		cachedNode = 0;
		cachedIndex = -1;
	}
	
	template<class ST> inline Node* firstNode(const ST& a) const;
	template<class ST> inline Node* lastNode(const ST& b) const;
	inline Node* minNode() { return (root) ? root->min() : 0; }
	inline Node* maxNode() { return (root) ? root->max() : 0; }
	
	int first(const T& a) const;
	int last(const T& b) const;
	
	inline int weight() const { return weight(root); }
	
	bool lookupByIndex(int index, Node** node = 0) const;
	template<class ST> Node* find(const ST& item, bool* found = 0, bool* below = 0, int* index = 0) const;
	
	Node* unlink(Node* k);
	static void clear(Node* k);
	static Node* clone(Node* k);
	
	void push(int index, const T& item);
	void pop(int index, T* item);
	
	void spliceInBefore(Node* kb, Node* kn);
	void spliceInAfter(Node* ka, Node* kn);
	
	void spliceIn(Node* kp, Node* kn, bool left);
	Node* spliceOut(Node* k);
	
	inline bool health() {
		// levelPrint();
		/*FTL_CHECK(ok1(root));*/
		FTL_CHECK(ok2(root));
		FTL_CHECK(ok3(root));
		FTL_CHECK(testBalance1(root));
		FTL_CHECK(testBalance2(root));
		FTL_CHECK(testWeight(root));
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
	inline static void updateWeight(Node* k) { k->weight_ = weight(k->left_) + weight(k->right_) + 1; }
	inline static int weight(Node* k) { return (k) ? k->weight_ : 0; }
	
	static bool ok1(Node* k);  // condition: k->left_->data() < k->right_->data()
	static bool ok2(Node* k);  // condition: (k == k->parent_()->left) || (k == k->parent_()->right)
	static bool ok3(Node* k);  // condition: (k == k->parent_()->left) || (k == k->parent_()->right)
	
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
typename BinaryTree<T>::Node* BinaryTree<T>::firstNode(const ST& a) const
{
	bool found = false, below = true;
	Node* k = find(a, &found, &below);
	if ((k == 0) || found) return k;
	return below ? k : k->succ();
}

template<class T>
template<class ST>
typename BinaryTree<T>::Node* BinaryTree<T>::lastNode(const ST& b) const
{
	bool found = false, below = true;
	Node* k = find(b, &found, &below);
	if ((k == 0) || found) return k;
	return below ? k->pred() : k;
}

template<class T>
inline int BinaryTree<T>::first(const T& a) const
{
	bool found = false, below = true;
	int index = 0;
	if (!root) return 0;
	find(a, &found, &below, &index);
	if (found) return index;
	return below ? index : index + 1;
}

template<class T>
inline int BinaryTree<T>::last(const T& b) const
{
	bool found = false, below = true;
	int index = 0;
	if (!root) return 0;
	find(b, &found, &below, &index);
	if (found) return index;
	return below ? index - 1 : index;
}

template<class T>
bool BinaryTree<T>::lookupByIndex(int i, Node** node) const
{
	if (i < 0) i += weight(root);
	FTL_CHECK((0 <= i) && (i < weight(root)));
	
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
	
	cachedNode = k;
	cachedIndex = i;
	
	return k;
}

template<class T>
template<class ST>
typename BinaryTree<T>::Node* BinaryTree<T>::find(const ST& item, bool* found, bool* below, int* index) const
{
	Node* k = root;
	Node* k2 = 0;
	if (found) *found = false;
	int j0 = 0, j = -1;
	if (k) while (true) {
		k2 = k;
		j = j0 + weight(k->left_);
		if (item < k->item_) {
			if (!k->left_) {
				if (below) *below = true;
				break;
			}
			k = k->left_;
		}
		else if (k->item_ < item) {
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

template<class T>
typename BinaryTree<T>::Node* BinaryTree<T>::unlink(Node* k)
{
	if (k->left_)
		replaceNode(k, spliceOut(k->left_->max()));
	else if (k->right_)
		replaceNode(k, spliceOut(k->right_->min()));
	else
		spliceOut(k);
	return k;
}

template<class T>
void BinaryTree<T>::clear(Node* k)
{
	if (!k) return;
	clear(k->left_);
	clear(k->right_);
	delete k;
}

template<class T>
typename BinaryTree<T>::Node* BinaryTree<T>::clone(Node* k)
{
	if (!k) return 0;
	Node* kn = new Node(k->item_, k->balance_, k->weight_);
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
		FTL_CHECK(found);
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
	FTL_CHECK(found);
	*item = ko->item_;
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
	else if (kb->left_)
		spliceIn(kb->left_->max(), kn, false/*left*/);
	else
		spliceIn(kb, kn, true/*left*/);
}

template<class T>
void BinaryTree<T>::spliceInAfter(Node* ka, Node* kn)
{
	if (!ka)
		spliceIn(ka, kn, true/*left*/);
	else if (ka->right_)
		spliceIn(ka->right_->min(), kn, true/*left*/);
	else
		spliceIn(ka, kn, false/*left*/);
}

template<class T>
void BinaryTree<T>::spliceIn(Node* kp, Node* kn, bool left)
{
	if (!kp) {
		// -- splice in initial node
		root = kn;
		kn->parent_ = 0;
		kn->left_ = 0;
		kn->right_ = 0;
	}
	else {
		// -- splice in new leaf node
		if (left)
			kp->left_ = kn;
		else
			kp->right_ = kn;
		kn->parent_ = kp;
		kn->left_ = 0;
		kn->right_ = 0;
		
		restoreWeights(kp, 1);
		rebalanceAfterSpliceIn(kp, kn);
	}
	
	cachedNode = 0;
}

template<class T>
typename BinaryTree<T>::Node* BinaryTree<T>::spliceOut(Node* k)
{
	Node* kp = k->parent_;

	if (!kp) {
		// -- splice out the root
		root = 0;
	}
	else {
		rebalanceBeforeSpliceOut(kp, k);
		
		// -- splice out node
		Node* kc = (k->left_ != 0) ? k->left_ : k->right_;
		if (kp->left_ == k)
			kp->left_ = kc;
		else
			kp->right_ = kc;
		if (kc != 0) kc->parent_ = kp;
		
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
		root = kl;
	if (ki->left_)
		ki->left_->parent_ = kl;
	if (ki->right_)
		ki->right_->parent_ = kl;
}

template<class T>
void BinaryTree<T>::rotateLeft(Node* k1)
{
	Node* k2 = k1->right_;
	
	if (k1->parent_ != 0)
	{
		if (k1->parent_->left_ == k1)
			k1->parent_->left_ = k2;
		else
			k1->parent_->right_ = k2;
	}
	else
		root = k2;
	
	k2->parent_ = k1->parent_;
	
	k1->parent_ = k2;
	k1->right_ = k2->left_;
	
	if (k2->left_ != 0)
		k2->left_->parent_ = k1;
	
	k2->left_ = k1;
	
	updateWeight(k1);
	updateWeight(k1->parent_);
}

template<class T>
void BinaryTree<T>::rotateRight(Node* k1)
{
	Node* k2 = k1->left_;
	
	if (k1->parent_ != 0)
	{
		if (k1->parent_->left_ == k1)
			k1->parent_->left_ = k2;
		else
			k1->parent_->right_ = k2;
	}
	else
		root = k2;
	
	k2->parent_ = k1->parent_;
	
	k1->parent_ = k2;
	k1->left_ = k2->right_;
	
	if (k2->right_ != 0)
		k2->right_->parent_ = k1;
	
	k2->right_ = k1;
	
	updateWeight(k1);
	updateWeight(k1->parent_);
}

template<class T>
void BinaryTree<T>::rebalanceAfterSpliceIn(Node* kp, Node* kn)
{
	int delta = 2 * (kp->left_ == kn) - 1;
	Node* k = kp;
	
	while (true)
	{
		k->balance_ += delta;
		
		if ((k->balance_ == 2) || (k->balance_ == -2))
			k = restoreBalance(k);
		
		if ((k->balance_ == 0) || (!k->parent_))
			break;
		
		delta = 2 * (k->parent_->left_ == k) - 1;
		k = k->parent_;
	}
}

template<class T>
void BinaryTree<T>::rebalanceBeforeSpliceOut(Node* kp, Node* ko)
{
	int delta = 2 * (kp->left_ == ko) - 1;
	Node* k = kp;
	
	while (true)
	{
		k->balance_ -= delta;
		
		if ((k->balance_ == 2) || (k->balance_ == -2))
			k = restoreBalance(k);
		
		if ((k->balance_ != 0) || (!k->parent_))
			break;
		
		delta = 2 * (k->parent_->left_ == k) - 1;
		k = k->parent_;
	}
}

template<class T>
typename BinaryTree<T>::Node* BinaryTree<T>::restoreBalance(Node* k1)
{
	if (k1->balance_ == 2)
	{
		Node* k2 = k1->left_;
		
		if (k2->balance_ == -1)
		{
			// -- case Ia->III
			Node* k3 = k2->right_;
			
			k1->balance_ = -(k3->balance_ == 1);
			k2->balance_ = (k3->balance_ == -1);
			k3->balance_ = 0;
			
			rotateLeft(k2);
			rotateRight(k1);
		}
		else
		{
			// -- case Ib, Ic
			k1->balance_ = (k2->balance_ <= 0);
			k2->balance_ = k2->balance_ - 1;
			
			rotateRight(k1);
		}
	}
	else
	{
		Node* k2 = k1->right_;
		
		if (k2->balance_ == 1)
		{
			// -- case IIc->IV
			Node* k3 = k2->left_;
			
			k1->balance_ = (k3->balance_ == -1);
			k2->balance_ = -(k3->balance_ == 1);
			k3->balance_ = 0;
			
			rotateRight(k2);
			rotateLeft(k1);
		}
		else
		{
			// -- case IIa, IIb
			k1->balance_ = -(k2->balance_ >= 0);
			k2->balance_ = k2->balance_ + 1;
			
			rotateLeft(k1);
		}
	}
	
	return k1->parent_;
}

template<class T>
void BinaryTree<T>::restoreWeights(Node* k, int delta)
{
	while (k) {
		k->weight_ += delta;
		k = k->parent_;
	}
}

template<class T>
int BinaryTree<T>::height(Node* k)
{
	if (k == 0) return 0;
	return max(height(k->left_), height(k->right_)) + 1;
}

template<class T>
bool BinaryTree<T>::ok1(Node* k)
{
	if (k == 0) return true;

	if (k->left_)
		if (!(k->left_->data() < k->data())) return false;

	if (k->right_)
		if (!(k->data() < k->right_->data())) return false;

	return ok1(k->left_) && ok1(k->right_);
}

template<class T>
bool BinaryTree<T>::ok2(Node* k)
{
	if (k == 0) return true;

	if (k->parent_)
		if (!((k == k->parent_->left_)||(k == k->parent_->right_))) return false;

	return ok2(k->left_) && ok2(k->right_);
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

	return ok3(k->left_) && ok3(k->right_);
}

template<class T>
bool BinaryTree<T>::testBalance1(Node* k)
{
	if (!k) return true;
	if (!((k->balance_ == -1) || (k->balance_ == 0) || (k->balance_ == 1))) return false;
	return testBalance1(k->left_) && testBalance1(k->right_);
}

template<class T>
bool BinaryTree<T>::testBalance2(Node* k)
{
	if (!k) return true;
	FTL_CHECK(((height(k->left_) - height(k->right_)) == k->balance_));
	if ((height(k->left_) - height(k->right_)) != k->balance_) return false;
	return testBalance2(k->left_) && testBalance2(k->right_);
}

template<class T>
bool BinaryTree<T>::testWeight(Node* k)
{
	if (!k) return true;
	return
		(weight(k->left_) + weight(k->right_) + 1 == k->weight_) &&
		testWeight(k->left_) && testWeight(k->right_);
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
		printf("(%p: %p,%p,%p,%d) ", k, k->parent_, k->left_, k->right_, k->balance_);
	}
	else {
		levelPrint(k->left_, level-1);
		levelPrint(k->right_, level-1);
	}
}*/

} // namespace ftl

#endif
