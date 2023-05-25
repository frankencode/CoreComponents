/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/buckets/BucketIndexTree>

namespace cc::buckets {

BucketIndexTree::Node *BucketIndexTree::stepDownTo(long index, unsigned *egress) const
{
    assert(0 <= index && index <= weight_);

    Node *node = root_;

    if (index + 1 >= weight_) {
        node = lastLeaf_;
        *egress = (node) ? node->fill_ - (index < weight_) : 0;
    }
    else if (index <= 0) {
        for (int h = height_; h > 0; --h) {
            node = static_cast<const Branch *>(node)->childAt(0);
        }
        *egress = 0;
    }
    else if (isDense()) {
        for (int h = height_; h > 0; --h) {
            node = static_cast<const Branch *>(node)->childAt((index >> (h * StepBits)) & StepMask);
        }
        *egress = index & StepMask;
    }
    else {
        for (int h = height_; h > 0; --h) {
            node = static_cast<const Branch *>(node)->find(index);
        }
        *egress = index;
    }

    return node;
}

void BucketIndexTree::joinSucc(Node *node, Node *newNode, bool isBranch)
{
    Node *oldSucc = node->succ();

    if (oldSucc) {
        newNode->succ_ = oldSucc;
        oldSucc->pred_ = newNode;
        setDenseOff();
    }
    else if (!isBranch) {
        lastLeaf_ = newNode;
    }

    newNode->pred_ = node;
    node->succ_ = newNode;

    Branch *parent = node->parent_;
    if (parent) {
        unsigned newNodeIndex = parent->indexOf(node) + 1;
        dissipate(parent, newNodeIndex);
        parent->push(newNodeIndex, newNode, 0);
    }
    else {
        Branch *branch = new Branch;
        branch->push(0, root_, weight_);
        branch->push(1, newNode, 0);
        root_ = branch;
        ++height_;
    }
}

void BucketIndexTree::unlink(Node *node, bool isBranch)
{
    Branch *parent = node->parent_;
    parent->drop(parent->indexOf(node));

    Node *succ = node->succ();
    Node *pred = node->pred();
    if (pred) pred->succ_ = succ;
    if (succ) succ->pred_ = pred;
    else if (!isBranch) lastLeaf_ = pred;

    delete node;

    relieve(parent);
}

void BucketIndexTree::shiftWeights(Node *from, Node *to, long delta)
{
    while (from != to) {
        weight(from) -= delta;
        weight(to) += delta;
        from = from->parent_;
        to = to->parent_;
    }
}

void BucketIndexTree::updateWeights(Node *node, long delta)
{
    for (int h = height_; h > 0; --h) {
        weight(node) += delta;
        node = node->parent_;
    }
    weight_ += delta;
}

void BucketIndexTree::reduce()
{
    if (root_) {
        while (height_ > 0 && root_->fill_ == 1) {
            Branch *branch = static_cast<Branch *>(root_);
            root_ = branch->childAt(0);
            delete branch;
            --height_;
        }
        root_->parent_ = nullptr;
    }
}

template void BucketIndexTree::dissipateSlow<BucketIndexTree::Branch>(Branch *&node, unsigned &egress);
template void BucketIndexTree::relieve<BucketIndexTree::Branch>(Branch *node);
template void BucketIndexTree::collapseSucc<BucketIndexTree::Branch>(Branch *node, Branch *succ);

} // namespace cc::buckets
