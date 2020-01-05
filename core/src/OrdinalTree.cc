/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/OrdinalTree>

namespace cc {

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
                restoreBalance(v[(i - 1) >> 1], i & 1, false);
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

    ExclusiveAccess cacheAccess{&cacheExclusive_};
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

    restoreBalance(kp, left, attached);

    cachedNode_ = nullptr;
}

void OrdinalTree::rotated(OrdinalNode *k1, bool /*left*/)
{
    establishWeight(k1);
    establishWeight(k1->parent_);
}

#ifndef NDEBUG

bool OrdinalTree::testWeight(OrdinalNode *k)
{
    if (!k) return true;
    return
        (weight(k->left_) + weight(k->right_) + 1 == k->weight_) &&
        testWeight(k->left_) && testWeight(k->right_);
}

#endif // ndef NDEBUG

} // namespace cc
