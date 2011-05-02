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
	
	bool lookupByIndex(int index, Node** node = 0) const;
	// int indexOf(Node* node) const; // TODO
	
protected:
	virtual void rotateLeft(Node* k);
	virtual void rotateRight(Node* k);
	virtual void rebalanceAfterSpliceIn(Node* k, int delta);
	virtual void rebalanceAfterSpliceOut(Node* k, int delta);
	
private:
	static void restoreWeights(Node* k, const int delta);
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
void OrdinalTree<T, M>::rebalanceAfterSpliceIn(Node* k, int delta)
{
	restoreWeights(k, 1);
	Super::rebalanceAfterSpliceIn(k, delta);
}

template<class T, class M>
void OrdinalTree<T, M>::rebalanceAfterSpliceOut(Node* k, int delta)
{
	restoreWeights(k, -1);
	Super::rebalanceAfterSpliceOut(k, delta);
}

template<class T, class M>
void OrdinalTree<T, M>::restoreWeights(Node* k, const int delta)
{
	while (k) {
		k->weight += delta;
		k = k->parent;
	}
}

} // namespace ftl

#endif // FTL_ORDINALTREE_HPP
