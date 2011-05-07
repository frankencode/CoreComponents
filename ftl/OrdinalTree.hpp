/*
 * OrdinalTree.hpp -- weighted binary trees with index access
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

template<class M>
class OrdinalMisc: public M {
public:
	OrdinalMisc(): weight(1) {}
	int weight;
};

template<class T, class M = None>
class OrdinalTree: public AvlTree<T, OrdinalMisc<M> > {
public:
	typedef AvlTree<T, OrdinalMisc<M> > Super;
	typedef typename Super::Node Node;
	
protected:
	bool lookupByIndex(int index, Node** node = 0) const;
	template<class ST>
	Node* find(const ST& e, bool* found = 0, int* index = 0) const;
	
	inline int numberOfNodes() const { return weight(Super::root::root); }
	inline int firstIndex() const { return (numberOfNodes() > 0) ? 0 : -1; }
	inline int lastIndex() const { return numberOfNodes() - 1; }
	
	virtual void rotateLeft(Node* k);
	virtual void rotateRight(Node* k);
	virtual void rebalanceAfterSpliceIn(Node* kp, Node* kn);
	virtual void rebalanceAfterSpliceOut(Node* kp, Node* ko);
	
private:
	static void restoreWeights(Node* k, int delta);
	inline static void updateWeight(Node* k) { k->weight = weight(k->left) + weight(k->right) + 1; }
	inline static int weight(Node* k) { return k ? k->weight : 0; }
};

template<class T, class M>
bool OrdinalTree<T, M>::lookupByIndex(int i, Node** node) const
{
	Node* k = Super::Super::root;
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
	return k;
}

template<class T, class M>
template<class ST>
typename OrdinalTree<T, M>::Node* OrdinalTree<T, M>::find(const ST& e, bool* found, int* index) const
{
	Node* k = Super::Super::root;
	Node* k2 = 0;
	int j0 = 0, j = -1;
	if (found) *found = false;
	while (k) {
		k2 = k;
		j = j0 + weight(k->left);
		if (e < k->data()) {
			k = k->left;
		}
		else if (k->data() < e) {
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

template<class T, class M>
void OrdinalTree<T, M>::rotateLeft(Node* k1)
{
	Super::rotateLeft(k1);
	updateWeight(k1);
	updateWeight(k1->parent);
}

template<class T, class M>
void OrdinalTree<T, M>::rotateRight(Node* k1)
{
	Super::rotateRight(k1);
	updateWeight(k1);
	updateWeight(k1->parent);
}

template<class T, class M>
void OrdinalTree<T, M>::rebalanceAfterSpliceIn(Node* kp, Node* kn)
{
	restoreWeights(kp, 1);
	Super::rebalanceAfterSpliceIn(kp, kn);
}

template<class T, class M>
void OrdinalTree<T, M>::rebalanceAfterSpliceOut(Node* kp, Node* ko)
{
	restoreWeights(kp, -1);
	Super::rebalanceAfterSpliceOut(kp, ko);
}

template<class T, class M>
void OrdinalTree<T, M>::restoreWeights(Node* k, int delta)
{
	while (k) {
		k->weight += delta;
		k = k->parent;
	}
}

} // namespace ftl

#endif // FTL_ORDINALTREE_HPP
