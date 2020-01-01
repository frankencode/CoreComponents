/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <string.h>
#include <cc/index>

namespace cc {
namespace index {

Branch::Branch()
{
    isBranch_ = 1;
    isIdeal_ = 1;
}

void Branch::push(unsigned nodeIndex, const Head *head)
{
    assert(fill_ < Capacity);
    assert(nodeIndex <= fill_);

    head->node_->parent_ = this;

    ::memmove(&head_[nodeIndex + 1], &head_[nodeIndex], (fill_ - nodeIndex) * sizeof(Local<Head>));
    head_[nodeIndex] = *head;
    ++fill_;
}

void Branch::pop(unsigned nodeIndex)
{
    assert(fill_ > 0);
    assert(nodeIndex < fill_);

    --fill_;
    ::memmove(&head_[nodeIndex], &head_[nodeIndex + 1], (fill_ - nodeIndex) * sizeof(Local<Head>));
}

void Branch::copyToPred(Branch *pred)
{
    assert(pred->fill_ + fill_ <= Node::Capacity);
    ::memcpy(&pred->head_[pred->fill_], &head_[0], fill_ * sizeof(Local<Head>));
    for (unsigned i = 0; i < fill_; ++i) pred->head_[pred->fill_ + i]->node_->parent_ = pred;
    pred->fill_ += fill_;
}

Path::Path():
    node_{nullptr}
{}

Path::Path(const Tree *tree, int64_t index, bool insertion):
    origin_{0},
    node_{tree->root_->node_},
    depth_{0},
    nodeIndex_{0}
{
    if (!node_) return;

    if (index + !insertion >= tree->root_->weight_) {
        while (node_->isBranch_) stepDown(node_->fill_ - 1);
        nodeIndex_ = node_->fill_ - !insertion;
    }
    else if (index <= 0) {
        while (node_->isBranch_) stepDown(0);
    }
    else {
        unsigned height = tree->height_;
        int64_t offset0 = 0;

        while (node_->isBranch_) {
            Branch *branch = static_cast<Branch *>(node_);
            if (branch->isIdeal_) {
                unsigned i = (index - offset0) >> (height << 2);
                stepDown(i);
                offset0 += i << (height << 2);
                --height;
            }
            else {
                for (unsigned i = 0; i < branch->fill_; ++i) {
                    int64_t offset1 = offset0 + branch->at(i)->weight_;
                    if (index < offset1) {
                        stepDown(i);
                        --height;
                        break;
                    }
                    offset0 = offset1;
                }
            }
        }

        nodeIndex_ = index - offset0;
    }

    assert(!node_->isBranch_);
}

bool Path::stepPred()
{
    Node *pred = nullptr;
    if (depth_ > 0 && hasPred()) {
        assert(node_);
        unsigned origin = getOrigin();
        if (origin > 0) {
            --origin;
            pred = node_->parent_->at(origin)->node_;
            setOrigin(origin);
        }
        else if (depth_ > 1) {
            Node *otherNode = node_;
            unsigned otherDepth = depth_;
            do {
                otherNode = otherNode->parent_;
                --otherDepth;
                origin = getOriginAtDepth(otherDepth);
                if (origin > 0) {
                    otherNode = otherNode->parent_->at(origin - 1)->node_;
                    setOriginAtDepth(otherDepth, origin - 1);
                    for (; otherDepth < depth_; ++otherDepth) {
                        setOriginAtDepth(otherDepth + 1, otherNode->fill_ - 1);
                        otherNode = static_cast<Branch *>(otherNode)->at(otherNode->fill_ - 1)->node_;
                    }
                    pred = otherNode;
                    break;
                }
            }
            while (otherDepth > 1);
        }
    }
    node_ = pred;
    return pred;
}

bool Path::stepSucc()
{
    Node *succ = nullptr;
    if (depth_ > 0) {
        assert(node_);
        unsigned origin = getOrigin();
        if (origin + 1 < node_->parent_->fill_) {
            ++origin;
            succ = node_->parent_->at(origin)->node_;
            setOrigin(origin);
        }
        else if (depth_ > 1) {
            Node *otherNode = node_;
            unsigned otherDepth = depth_;
            do {
                otherNode = otherNode->parent_;
                --otherDepth;
                origin = getOriginAtDepth(otherDepth);
                if (origin + 1 < otherNode->parent_->fill_) {
                    otherNode = otherNode->parent_->at(origin + 1)->node_;
                    setOriginAtDepth(otherDepth, origin + 1);
                    for (; otherDepth < depth_; ++otherDepth) {
                        setOriginAtDepth(otherDepth + 1, 0);
                        otherNode = static_cast<Branch *>(otherNode)->at(0)->node_;
                    }
                    succ = otherNode;
                    break;
                }
            }
            while (otherDepth > 1);
        }
    }
    node_ = succ;
    return succ;
}

void Path::stepDown(unsigned egress)
{
    assert(egress < Node::Capacity);
    assert(depth_ < MaxDepth);
    assert(node_->isBranch_);

    node_ = static_cast<Branch *>(node_)->at(egress)->node_;
    origin_ |= (uint64_t(egress) << (depth_ << 2));
    nodeIndex_ = 0;
    ++depth_;
}

void Path::stepUp()
{
    assert(depth_ > 0);

    node_ = node_->parent_;
    nodeIndex_ = getOrigin();
    --depth_;
    origin_ &= ~(UINT64_C(0xF) << (depth_ << 2));
}

bool Path::iterateForward()
{
    if (node_) {
        if (nodeIndex_ + 1 < node_->fill_)
            ++nodeIndex_;
        else if (stepSucc())
            nodeIndex_ = 0;
    }
    return node_;
}

bool Path::iterateBackward()
{
    if (node_) {
        if (nodeIndex_ > 0)
            --nodeIndex_;
        else if (stepPred())
            nodeIndex_ = node_->fill_ - 1;
    }
    return node_;
}

unsigned Path::getOrigin() const
{
    return getOriginAtDepth(depth_);
}

void Path::setOrigin(unsigned newValue)
{
    setOriginAtDepth(depth_, newValue);
}

unsigned Path::getOriginAtDepth(unsigned depth) const
{
    assert(depth > 0);

    return (origin_ >> ((depth - 1) << 2)) & 0xFu;
}

void Path::setOriginAtDepth(unsigned depth, unsigned newValue)
{
    assert(depth > 0);
    assert(newValue <= 0xFu);

    unsigned depthShift = (depth - 1) << 2;

    origin_ &= ~(UINT64_C(0xF) << depthShift);
    origin_ |= uint64_t(newValue) << depthShift;
}

void Tree::updateWeights(const Path *path, int64_t delta, unsigned minDepth)
{
    Node *node = path->node();
    for (unsigned i = path->depth(); i > minDepth; --i) {
        Branch *parent = node->parent_;
        assert(parent);
        unsigned origin = path->getOriginAtDepth(i);
        const int64_t newWeight = (parent->at(origin)->weight_ += delta);
        if (delta < 0) {
            if (origin + 1 < parent->fill_) parent->isIdeal_ = 0;
        }
        else {
            const int64_t idealWeight = UINT64_C(1) << ((height_ - i + 1) << 2);
            if (newWeight == idealWeight && !node->isIdeal_) node->isIdeal_ = 1;
        }
        node = parent;
    }
}

void Tree::joinSucc(Path *path, Node *newNode)
{
    assert(path);
    assert(newNode->fill_ == 0);

    if (path->hasParent()) {
        Local<Path> parent = path->getParent();
        parent->setNodeIndex(parent->nodeIndex() + 1);
        unsigned heightSaved = height_;
        dissipate<Branch>(&parent);
        if (height_ != heightSaved) path->grow();
        parent->branch()->push(parent->nodeIndex(), Local<Head>{.weight_ = 0, .node_ = newNode});
    }
    else {
        assert(path->node() == root_->node_);
        Branch *branch = new Branch;
        branch->isIdeal_ = root_->node_->isIdeal_;
        branch->push(0, root_);
        branch->push(1, Local<Head>{.weight_ = 0, .node_ = newNode});
        root_ = Head{.weight_ = root_->weight_, .node_ = branch};
        ++height_;
        path->grow();
    }
}

unsigned Tree::commonDepth(const Path *first, const Path *second)
{
    assert(first->depth() == second->depth());
    unsigned depth = 1;
    while (depth < 16 && first->getOriginAtDepth(depth) == second->getOriginAtDepth(depth)) ++depth;
    return depth - 1;
}

void Tree::reduce()
{
    while (root_->node_ && root_->node_->isBranch_ && root_->node_->fill_ == 1) {
        Branch *branch = static_cast<Branch *>(root_->node_);
        root_->node_ = branch->at(0)->node_;
        delete branch;
        --height_;
    }
}

String Tree::dotify() const
{
    Format format;
    format
        << "digraph g {\n"
        << "graph [\n"
        << "rankdir = \"LR\"\n"
        << "]\n";
    dotifyNode(format, root_);
    format << "}\n";
    return format;
}

void Tree::dotifyNode(Format &format, const Head *head, unsigned origin) const
{
    const Node *node = head->node_;
    if (node->isBranch_) {
        format
            << "branch_" << (void *)node << " [\n"
            << "label = \"<f0>origin: " << origin << "|<f1>weight: " << head->weight_ << "|<f2>fill: " << node->fill_ << "|<f3>isIdeal: " << (node->isIdeal_ == 1) << "\"\n"
            << "shape = \"record\"\n"
            << "];\n";

        const Branch *branch = static_cast<const Branch *>(node);
        for (unsigned i = 0; i < branch->fill_; ++i) {
            dotifyNode(format, branch->at(i), i);
        }
        for (unsigned i = 0; i < branch->fill_; ++i) {
            Node *child = branch->at(i)->node_;
            format << "branch_" << (void *)node << " -> " << (child->isBranch_ ? "branch_" : "node_") << (void *)child << ":f0;" << nl;
        }
    }
    else {
        format
            << "node_" << (void *)node << " [\n"
            << "label = \"<f0>origin: " << origin << "|<f1>weight: " << head->weight_ << "|<f2>fill: " << node->fill_ << "\"\n"
            << "shape = \"record\"\n"
            << "];\n";
    }
}

}} // namespace cc::index
