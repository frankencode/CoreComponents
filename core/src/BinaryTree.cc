/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/BinaryTree>

namespace cc {

void AvlBalance::restore(BinaryTree *tree, BinaryNode *kp, bool left, bool attached)
{
    if (!kp) return;

    int delta = 2 * left - 1;
    BinaryNode *k = kp;
    while (true) {
        k->balance_ += attached ? delta : -delta;
        if ((k->balance_ == 2) || (k->balance_ == -2))
            k = rebalance(tree, k);
        if ( (attached ? (k->balance_ == 0) : (k->balance_ != 0)) ||
             (!k->parent_) )
            break;
        delta = 2 * (k->parent_->left_ == k) - 1;
        k = k->parent_;
    }
}

BinaryNode *AvlBalance::rebalance(BinaryTree *tree, BinaryNode *k1)
{
    if (k1->balance_ == 2) {
        BinaryNode *k2 = k1->left_;
        if (k2->balance_ == -1) {
            // -- case Ia->III
            BinaryNode *k3 = k2->right_;
            k1->balance_ = -(k3->balance_ == 1);
            k2->balance_ = (k3->balance_ == -1);
            k3->balance_ = 0;
            tree->rotate(k2, true);
            tree->rotate(k1, false);
        }
        else {
            // -- case Ib, Ic
            k1->balance_ = (k2->balance_ <= 0);
            k2->balance_ = k2->balance_ - 1;
            tree->rotate(k1, false);
        }
    }
    else {
        BinaryNode *k2 = k1->right_;
        if (k2->balance_ == 1) {
            // -- case IIc->IV
            BinaryNode *k3 = k2->left_;
            k1->balance_ = (k3->balance_ == -1);
            k2->balance_ = -(k3->balance_ == 1);
            k3->balance_ = 0;
            tree->rotate(k2, false);
            tree->rotate(k1, true);
        }
        else {
            // -- case IIa, IIb
            k1->balance_ = -(k2->balance_ >= 0);
            k2->balance_ = k2->balance_ + 1;
            tree->rotate(k1, true);
        }
    }
    return k1->parent_;
}

#ifndef NDEBUG

int AvlBalance::height(BinaryNode *k)
{
    if (k == 0) return 0;
    int h1 = height(k->left_);
    int h2 = height(k->right_);
    return (h1 < h2 ? h2 : h1) + 1;
}

bool AvlBalance::testBalance(BinaryNode *k)
{
    if (!k) return true;
    if (!((k->balance_ == -1) || (k->balance_ == 0) || (k->balance_ == 1))) return false;
    if ((height(k->left_) - height(k->right_)) != k->balance_) return false;
    return testBalance(k->left_) && testBalance(k->right_);
}

#endif // ndef NDEBUG

/** Attaches the new leaf node k under kp as the left or right child.
  */
void BinaryTree::attach(BinaryNode *kp, BinaryNode *k, bool left)
{
    if (kp) {
        if (left)
            kp->left_ = k;
        else
            kp->right_ = k;
        k->parent_ = kp;
        k->left_ = 0;
        k->right_ = 0;
    }
    else {
        root_ = k;
        k->parent_ = 0;
        k->left_ = 0;
        k->right_ = 0;
    }
    changed(kp, k, left, true);
}

/** Detaches the leaf or list node k from the tree.
  */
BinaryNode *BinaryTree::detach(BinaryNode *k)
{
    BinaryNode *kp = k->parent_;
    bool left = false;
    if (kp) {
        BinaryNode *kc = (k->left_ != 0) ? k->left_ : k->right_;
        left = (kp->left_ == k);
        if (left)
            kp->left_ = kc;
        else
            kp->right_ = kc;
        if (kc != 0) kc->parent_ = kp;
    }
    else {
        root_ = 0;
    }
    changed(kp, k, left, false);
    return k;
}

/** Put the leaf node kl into the place of inner node ki
  */
void BinaryTree::replace(BinaryNode *ki, BinaryNode *kl)
{
    *kl = *ki;

    // establish links from neighbors
    BinaryNode *kp = ki->parent_;
    if (kp) {
        if (kp->left_ == ki)
            kp->left_ = kl;
        else
            kp->right_ = kl;
    }
    else {
        root_ = kl;
    }
    if (ki->left_)
        ki->left_->parent_ = kl;
    if (ki->right_)
        ki->right_->parent_ = kl;
}

/** Remove the node k from the tree.
  */
BinaryNode *BinaryTree::unlink(BinaryNode *k)
{
    if (k->left_)
        replace(k, detach(max(k->left_)));
    else if (k->right_)
        replace(k, detach(min(k->right_)));
    else
        detach(k);
    return k;
}

BinaryNode *BinaryTree::pred(BinaryNode *k)
{
    if (k->left_)
        return max(k->left_);
    BinaryNode *kp = k->parent_;
    while (kp) {
        if (k == kp->right_) break;
        k = kp;
        kp = kp->parent_;
    }
    return kp;
}

BinaryNode *BinaryTree::succ(BinaryNode *k)
{
    if (k->right_)
        return min(k->right_);
    BinaryNode *kp = k->parent_;
    while (kp) {
        if (k == kp->left_) break;
        k = kp;
        kp = kp->parent_;
    }
    return kp;
}

BinaryNode *BinaryTree::min(BinaryNode *k)
{
    BinaryNode *k2 = k;
    while (k) {
        k2 = k;
        k = k->left_;
    }
    return k2;
}

BinaryNode *BinaryTree::max(BinaryNode *k)
{
    BinaryNode *k2 = k;
    while (k) {
        k2 = k;
        k = k->right_;
    }
    return k2;
}

/** Perform an equivalent tree transformation.
  *
  * Rotate left:
  *
  *    k1      =>      k2
  *   /  \            /  \
  *  a    k2         k1   c
  *      /  \       /  \
  *     b    c     a    b
  *
  * Rotate right:
  *
  *      k1    =>    k2
  *     /  \        /  \
  *    k2   c      a    k1
  *   /  \             /  \
  *  a    b           b    c
  */
void BinaryTree::rotate(BinaryNode *k1, bool left)
{
    BinaryNode *k2 = left ? k1->right_ : k1->left_;
    if (k1->parent_) {
        if (k1->parent_->left_ == k1)
            k1->parent_->left_ = k2;
        else
            k1->parent_->right_ = k2;
    }
    else {
        root_ = k2;
    }
    k2->parent_ = k1->parent_;
    k1->parent_ = k2;
    if (left) {
        k1->right_ = k2->left_;
        if (k2->left_)
            k2->left_->parent_ = k1;
        k2->left_ = k1;
    }
    else {
        k1->left_ = k2->right_;
        if (k2->right_)
            k2->right_->parent_ = k1;
        k2->right_ = k1;
    }
    rotated(k1, left);
}

void BinaryTree::populate(BinaryNode **v, int n)
{
    for (int i = 0, m = 1; i < n; m *= 2)
    {
        for (int i1 = i + m; i < i1; ++i)
        {
            if (i < n) {
                BinaryNode *k = v[i];
                BinaryNode *kp = 0;
                if (i > 0) kp = v[(i - 1) >> 1];
                k->parent_ = kp;
                if (kp) {
                    if (i & 1)
                        kp->left_ = k;
                    else
                        kp->right_ = k;
                }
                k->left_ = 0;
                k->right_ = 0;
            }
            else {
                AvlBalance::restore(this, v[(i - 1) >> 1], i & 1, false);
            }
        }
    }

    for (int i = n - 1; i > 0; --i) {
        BinaryNode *k = v[i];
        BinaryNode *kp = k->parent_;
        kp->weight_ += k->weight_;
    }

    root_ = v[0];

    /*
    CC_ASSERT(testStructure(root_));
    CC_ASSERT(testIteration(root_));
    CC_ASSERT(testBalance(root_));
    CC_ASSERT(testWeight(root_));*/
}

BinaryNode *BinaryTree::getNodeAt(int i) const
{
    CC_ASSERT((0 <= i) && (i < weight()));

    ExclusiveAccess cacheAccess(&cacheExclusive_);
    if (cacheAccess) {
        if (cachedNode_) {
            const int d = i - cachedIndex_;
            if (d == 1) {
                ++cachedIndex_;
                cachedNode_ = succ(cachedNode_);
                return cachedNode_;
            }
            else if (d == 0) {
                return cachedNode_;
            }
            else if (d == -1) {
                --cachedIndex_;
                cachedNode_ = pred(cachedNode_);
                return cachedNode_;
            }
        }
    }

    BinaryNode *k = root_;
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

    if (cacheAccess) {
        cachedNode_ = k;
        cachedIndex_ = i;
    }

    return k;
}

void BinaryTree::attachBefore(BinaryNode *kb, BinaryNode *kn)
{
    if (!kb)
        BinaryTree::attach(kb, kn, true);
    else if (kb->left_)
        BinaryTree::attach(max(kb->left_), kn, false);
    else
        BinaryTree::attach(kb, kn, true);
}

#if 0
void BinaryTree::attachAfter(BinaryNode *ka, BinaryNode *kn)
{
    if (!ka)
        BinaryTree::attach(ka, kn, true);
    else if (ka->right_)
        BinaryTree::attach(min(ka->right_), kn, true);
    else
        BinaryTree::attach(ka, kn, false);
}
#endif

void BinaryTree::attachAt(int i, BinaryNode *kn)
{
    if (i == weight()) {
        BinaryNode *kp = 0;
        if (cachedNode_) {
            if (cachedIndex_ == i)
                kp = cachedNode_;
        }
        if (!kp) kp = max();
        attach(kp, kn, false);
    }
    else {
        BinaryNode *ka = getNodeAt(i);
        CC_ASSERT(ka);
        attachBefore(ka, kn);
    }

    cachedNode_ = kn;
    cachedIndex_ = i;
}

BinaryNode *BinaryTree::unlinkAt(int i)
{
    BinaryNode *ko = getNodeAt(i);
    CC_ASSERT(ko);
    BinaryNode *k = pred(ko);
    if (k) --i;
    else k = succ(ko);
    unlink(ko);
    if (k) {
        cachedNode_ = k;
        cachedIndex_ = i;
    }
    return ko;
}

void BinaryTree::changed(BinaryNode *kp, BinaryNode *kc, bool left, bool attached)
{
    int delta = attached ? 1 : -1;
    BinaryNode *k = kp;
    while (k) {
        k->weight_ += delta;
        k = k->parent_;
    }

    AvlBalance::restore(this, kp, left, attached);

    cachedNode_ = 0;
}

void BinaryTree::rotated(BinaryNode *k1, bool /*left*/)
{
    establishWeight(k1);
    establishWeight(k1->parent_);
}

#ifndef NDEBUG

bool BinaryTree::testStructure(BinaryNode *k)
{
    if (!k) return true;
    if (k->parent_) {
        if (!((k == k->parent_->left_) || (k == k->parent_->right_)))
            return false;
    }
    return testStructure(k->left_) && testStructure(k->right_);
}

bool BinaryTree::testWeight(BinaryNode *k)
{
    if (!k) return true;
    return
        (weight(k->left_) + weight(k->right_) + 1 == k->weight_) &&
        testWeight(k->left_) && testWeight(k->right_);
}

bool BinaryTree::testIteration(BinaryNode *k)
{
    if (k == 0) return true;
    BinaryNode *k2;
    k2 = succ(k);
    if (k2) {
        if (k != pred(k2))
            return false;
    }
    k2 = pred(k);
    if (k2) {
        if (k != succ(k2))
            return false;
    }
    return testIteration(k->left_) && testIteration(k->right_);
}

#endif // ndef NDEBUG

} // namespace cc
