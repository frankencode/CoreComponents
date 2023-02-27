/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/bucket/Tree>

namespace cc {
namespace bucket {

Node *Tree::stepDownTo(long index, unsigned *egress) const
{
    assert(0 <= index && index <= root_.weight_);

    Node *node = root_.node_;

    if (index + 1 >= root_.weight_) {
        node = lastLeaf_;
        *egress = (node) ? node->fill_ - (index < root_.weight_) : 0;
    }
    else if (index <= 0) {
        for (int h = height_; h > 0; --h) node = static_cast<const Branch *>(node)->at(0).node_;
        *egress = 0;
    }
    else if (isDense_ && Capacity == 16) {
        *egress = index & 0xFu;
        for (int h = height_; h > 0; --h) {
            node = static_cast<const Branch *>(node)->at((index >> (h << 2)) & 0xFu).node_;
        }
    }
    else {
        for (int h = height_; h > 0; --h) {
            const Branch *branch = static_cast<const Branch *>(node);
            const unsigned n = branch->fill_;
            for (unsigned i = 0; i < n; ++i) {
                const Head *head = &branch->at(i);
                index -= head->weight_;
                if (index < 0) {
                    node = head->node_;
                    index += head->weight_;
                    break;
                }
            }
        }
        *egress = index;
    }

    return node;
}

void Tree::joinSucc(Node *node, Node *newNode, bool isBranch)
{
    Node *oldSucc = node->succ();

    if (oldSucc) {
        newNode->succ_ = oldSucc;
        oldSucc->pred_ = newNode;
        isDense_ = false;
    }
    else if (!isBranch) {
        lastLeaf_ = newNode;
    }

    newNode->pred_ = node;
    node->succ_ = newNode;

    Branch *parent = node->parent_;
    if (parent) {
        unsigned newNodeIndex = node->parent_->indexOf(node) + 1;
        dissipate(parent, newNodeIndex);
        parent->push(newNodeIndex, Head{.weight_ = 0, .node_ = newNode});
    }
    else {
        Branch *branch = new Branch;
        branch->push(0, root_);
        branch->push(1, Head{.weight_ = 0, .node_ = newNode});
        root_.node_ = branch;
        ++height_;
    }
}

void Tree::unlink(Node *node, bool isBranch)
{
    Branch *parent = node->parent_;
    parent->pop(parent->indexOf(node));

    Node *succ = node->succ();
    Node *pred = node->pred();
    if (pred) pred->succ_ = succ;
    if (succ) succ->pred_ = pred;
    else if (!isBranch) lastLeaf_ = pred;

    delete node;

    relieve(parent);
}

void Tree::shiftWeights(Node *from, Node *to, long delta)
{
    while (from != to) {
        weight(from) -= delta;
        weight(to) += delta;
        from = from->parent_;
        to = to->parent_;
    }
}

void Tree::updateWeights(Node *node, long delta)
{
    for (int h = height_; h > 0; --h) {
        weight(node) += delta;
        node = node->parent_;
    }
    root_.weight_ += delta;
}

void Tree::reduce()
{
    if (root_.node_) {
        while (height_ > 0 && root_.node_->fill_ == 1) {
            Branch *branch = static_cast<Branch *>(root_.node_);
            root_.node_ = branch->at(0).node_;
            delete branch;
            --height_;
        }
        root_.node_->parent_ = nullptr;
    }
}

template void Tree::dissipate<Branch>(Branch *&node, unsigned &egress);
template void Tree::relieve<Branch>(Branch *node, bool recursive);
template void Tree::dissipateForward<Branch>(Branch *from, Branch *to);
template void Tree::dissipateBackward<Branch>(Branch *to, Branch *from);
template void Tree::distributeForward<Branch>(Branch *from, Branch *to);
template void Tree::collapseSucc<Branch>(Branch *node, Branch *succ);

}} // namespace cc::bucket
