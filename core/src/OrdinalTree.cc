/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/OrdinalTree>

namespace cc {

void AvlBalance::restore(OrdinalTree *tree, OrdinalNode *kp, bool left, bool attached)
{
    if (!kp) return;

    int delta = 2 * left - 1;
    OrdinalNode *k = kp;
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

OrdinalNode *AvlBalance::rebalance(OrdinalTree *tree, OrdinalNode *k1)
{
    if (k1->balance_ == 2) {
        OrdinalNode *k2 = k1->left_;
        if (k2->balance_ == -1) {
            // -- case Ia->III
            OrdinalNode *k3 = k2->right_;
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
        OrdinalNode *k2 = k1->right_;
        if (k2->balance_ == 1) {
            // -- case IIc->IV
            OrdinalNode *k3 = k2->left_;
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

int AvlBalance::height(OrdinalNode *k)
{
    if (k == nullptr) return 0;
    int h1 = height(k->left_);
    int h2 = height(k->right_);
    return (h1 < h2 ? h2 : h1) + 1;
}

bool AvlBalance::testBalance(OrdinalNode *k)
{
    if (!k) return true;
    if (!((k->balance_ == -1) || (k->balance_ == 0) || (k->balance_ == 1))) return false;
    if ((height(k->left_) - height(k->right_)) != k->balance_) return false;
    return testBalance(k->left_) && testBalance(k->right_);
}

#endif // ndef NDEBUG

/** Attaches the new leaf node k under kp as the left or right child.
  */
void OrdinalTree::attach(OrdinalNode *kp, OrdinalNode *k, bool left)
{
    if (kp) {
        if (left)
            kp->left_ = k;
        else
            kp->right_ = k;
        k->parent_ = kp;
        k->left_ = nullptr;
        k->right_ = nullptr;
    }
    else {
        root_ = k;
        k->parent_ = nullptr;
        k->left_ = nullptr;
        k->right_ = nullptr;
    }
    changed(kp, k, left, true);
}

/** Detaches the leaf or list node k from the tree.
  */
OrdinalNode *OrdinalTree::detach(OrdinalNode *k)
{
    OrdinalNode *kp = k->parent_;
    bool left = false;
    if (kp) {
        OrdinalNode *kc = (k->left_ != nullptr) ? k->left_ : k->right_;
        left = (kp->left_ == k);
        if (left)
            kp->left_ = kc;
        else
            kp->right_ = kc;
        if (kc != nullptr) kc->parent_ = kp;
    }
    else {
        root_ = nullptr;
    }
    changed(kp, k, left, false);
    return k;
}

/** Put the leaf node kl into the place of inner node ki
  */
void OrdinalTree::replace(OrdinalNode *ki, OrdinalNode *kl)
{
    // establish links from to kl to the neighbors of ki (and take over weight and balance, but not value of ki)
    *kl = *ki;

    // establish links from neighbors to kl
    OrdinalNode *kp = ki->parent_;
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
OrdinalNode *OrdinalTree::unlink(OrdinalNode *k)
{
    if (!k->left_ && !k->right_)
        detach(k);
    else if (!k->left_)
        replace(k, detach(min(k->right_)));
    else if (!k->right_)
        replace(k, detach(max(k->left_)));
    else if (k->left_->weight_ < k->right_->weight_)
        replace(k, detach(min(k->right_)));
    else
        replace(k, detach(max(k->left_)));

    return k;
}

int OrdinalTree::getIndexOf(OrdinalNode *k)
{
    int i = weight(k->left_);
    while (true) {
        OrdinalNode *kp = k->parent_;
        if (!kp) break;
        if (k == kp->right_)
            i += weight(kp->left_) + 1;
        k = kp;
    }
    return i;
}

OrdinalNode *OrdinalTree::pred(OrdinalNode *k)
{
    if (k->left_)
        return max(k->left_);
    OrdinalNode *kp = k->parent_;
    while (kp) {
        if (k == kp->right_) break;
        k = kp;
        kp = kp->parent_;
    }
    return kp;
}

OrdinalNode *OrdinalTree::succ(OrdinalNode *k)
{
    if (k->right_)
        return min(k->right_);
    OrdinalNode *kp = k->parent_;
    while (kp) {
        if (k == kp->left_) break;
        k = kp;
        kp = kp->parent_;
    }
    return kp;
}

OrdinalNode *OrdinalTree::min(OrdinalNode *k)
{
    OrdinalNode *k2 = k;
    while (k) {
        k2 = k;
        k = k->left_;
    }
    return k2;
}

OrdinalNode *OrdinalTree::max(OrdinalNode *k)
{
    OrdinalNode *k2 = k;
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
void OrdinalTree::rotate(OrdinalNode *k1, bool left)
{
    OrdinalNode *k2 = left ? k1->right_ : k1->left_;
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

void OrdinalTree::populate(OrdinalNode **v, int n)
{
    for (int i = 0, m = 1; i < n; m *= 2)
    {
        for (int i1 = i + m; i < i1; ++i)
        {
            if (i < n) {
                OrdinalNode *k = v[i];
                OrdinalNode *kp = nullptr;
                if (i > 0) kp = v[(i - 1) >> 1];
                k->parent_ = kp;
                if (kp) {
                    if (i & 1)
                        kp->left_ = k;
                    else
                        kp->right_ = k;
                }
                k->left_ = nullptr;
                k->right_ = nullptr;
            }
            else {
                AvlBalance::restore(this, v[(i - 1) >> 1], i & 1, false);
            }
        }
    }

    for (int i = n - 1; i > 0; --i) {
        OrdinalNode *k = v[i];
        OrdinalNode *kp = k->parent_;
        kp->weight_ += k->weight_;
    }

    root_ = v[0];

    /*
    CC_ASSERT(testStructure(root_));
    CC_ASSERT(testIteration(root_));
    CC_ASSERT(testBalance(root_));
    CC_ASSERT(testWeight(root_));*/
}

OrdinalNode *OrdinalTree::getNodeAt(int i) const
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

    OrdinalNode *k = root_;
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

void OrdinalTree::attachBefore(OrdinalNode *kb, OrdinalNode *kn)
{
    if (!kb)
        OrdinalTree::attach(kb, kn, true);
    else if (kb->left_)
        OrdinalTree::attach(max(kb->left_), kn, false);
    else
        OrdinalTree::attach(kb, kn, true);
}

#if 0
void OrdinalTree::attachAfter(OrdinalNode *ka, OrdinalNode *kn)
{
    if (!ka)
        OrdinalTree::attach(ka, kn, true);
    else if (ka->right_)
        OrdinalTree::attach(min(ka->right_), kn, true);
    else
        OrdinalTree::attach(ka, kn, false);
}
#endif

void OrdinalTree::attachAt(int i, OrdinalNode *kn)
{
    if (i == weight()) {
        OrdinalNode *kp = nullptr;
        if (cachedNode_) {
            if (cachedIndex_ == i)
                kp = cachedNode_;
        }
        if (!kp) kp = max();
        attach(kp, kn, false);
    }
    else {
        OrdinalNode *ka = getNodeAt(i);
        CC_ASSERT(ka);
        attachBefore(ka, kn);
    }

    cachedNode_ = kn;
    cachedIndex_ = i;
}

OrdinalNode *OrdinalTree::unlinkAt(int i)
{
    OrdinalNode *ko = getNodeAt(i);
    CC_ASSERT(ko);
    OrdinalNode *k = pred(ko);
    if (k) --i;
    else k = succ(ko);
    unlink(ko);
    if (k) {
        cachedNode_ = k;
        cachedIndex_ = i;
    }
    return ko;
}

void OrdinalTree::changed(OrdinalNode *kp, OrdinalNode *kc, bool left, bool attached)
{
    int delta = attached ? 1 : -1;
    OrdinalNode *k = kp;
    while (k) {
        k->weight_ += delta;
        k = k->parent_;
    }

    AvlBalance::restore(this, kp, left, attached);

    cachedNode_ = nullptr;
}

void OrdinalTree::rotated(OrdinalNode *k1, bool /*left*/)
{
    establishWeight(k1);
    establishWeight(k1->parent_);
}

#ifndef NDEBUG

bool OrdinalTree::testStructure(OrdinalNode *k)
{
    if (!k) return true;
    if (k->parent_) {
        if (!((k == k->parent_->left_) || (k == k->parent_->right_)))
            return false;
    }
    return testStructure(k->left_) && testStructure(k->right_);
}

bool OrdinalTree::testWeight(OrdinalNode *k)
{
    if (!k) return true;
    return
        (weight(k->left_) + weight(k->right_) + 1 == k->weight_) &&
        testWeight(k->left_) && testWeight(k->right_);
}

bool OrdinalTree::testIteration(OrdinalNode *k)
{
    if (k == 0) return true;
    OrdinalNode *k2;
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
