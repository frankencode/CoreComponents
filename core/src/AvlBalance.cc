/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/AvlBalance>

namespace cc {

void AvlBalance::restore(BinaryNode *kp, BinaryNode*, bool left, bool attached)
{
    if (!kp) return;

    int delta = 2 * left - 1;
    BinaryNode *k = kp;
    while (true) {
        k->balance_ += attached ? delta : -delta;
        if ((k->balance_ == 2) || (k->balance_ == -2))
            k = rebalance(k);
        if ( (attached ? (k->balance_ == 0) : (k->balance_ != 0)) ||
             (!k->parent_) )
            break;
        delta = 2 * (k->parent_->left_ == k) - 1;
        k = k->parent_;
    }
}

BinaryNode *AvlBalance::rebalance(BinaryNode *k1)
{
    if (k1->balance_ == 2) {
        BinaryNode *k2 = k1->left_;
        if (k2->balance_ == -1) {
            // -- case Ia->III
            BinaryNode *k3 = k2->right_;
            k1->balance_ = -(k3->balance_ == 1);
            k2->balance_ = (k3->balance_ == -1);
            k3->balance_ = 0;
            rotate(k2, true);
            rotate(k1, false);
        }
        else {
            // -- case Ib, Ic
            k1->balance_ = (k2->balance_ <= 0);
            k2->balance_ = k2->balance_ - 1;
            rotate(k1, false);
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
            rotate(k2, false);
            rotate(k1, true);
        }
        else {
            // -- case IIa, IIb
            k1->balance_ = -(k2->balance_ >= 0);
            k2->balance_ = k2->balance_ + 1;
            rotate(k1, true);
        }
    }
    return k1->parent_;
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
void AvlBalance::rotate(BinaryNode *k1, bool left)
{
    BinaryNode *k2 = left ? k1->right_ : k1->left_;
    if (k1->parent_) {
        if (k1->parent_->left_ == k1)
            k1->parent_->left_ = k2;
        else
            k1->parent_->right_ = k2;
    }
    else {
        setRoot(k2);
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

} // namespace cc
