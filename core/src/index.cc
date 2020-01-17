/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/index>

namespace cc {
namespace index {

void Branch::push(unsigned nodeIndex, const Local<Head> &head)
{
    assert(fill_ < Capacity);
    assert(nodeIndex <= fill_);

    head->node_->parent_ = this;

    if (nodeIndex < fill_) {
        ::memmove(&weight_[nodeIndex + 1], &weight_[nodeIndex], (fill_ - nodeIndex) * sizeof(Weight));
        ::memmove(&node_[nodeIndex + 1], &node_[nodeIndex], (fill_ - nodeIndex) * sizeof(Node *));
    }
    weight_[nodeIndex] = head->weight_;
    node_[nodeIndex] = head->node_;
    ++fill_;
}

void Branch::pop(unsigned nodeIndex)
{
    assert(fill_ > 0);
    assert(nodeIndex < fill_);

    --fill_;
    if (nodeIndex < fill_) {
        ::memmove(&weight_[nodeIndex], &weight_[nodeIndex + 1], (fill_ - nodeIndex) * sizeof(Weight));
        ::memmove(&node_[nodeIndex], &node_[nodeIndex + 1], (fill_ - nodeIndex) * sizeof(Node *));
    }
}

void Branch::copyToPred(Branch *pred)
{
    assert(pred->fill_ + fill_ <= Node::Capacity);
    ::memcpy(&pred->weight_[pred->fill_], &weight_[0], fill_ * sizeof(Weight));
    ::memcpy(&pred->node_[pred->fill_], &node_[0], fill_ * sizeof(Node *));
    for (unsigned i = 0; i < fill_; ++i) pred->node_[pred->fill_ + i]->parent_ = pred;
    pred->fill_ += fill_;
}

bool Path::stepPred()
{
    Node *pred = nullptr;
    if (depth_ > 0 && hasPred()) {
        assert(node_);
        unsigned origin = getOrigin();
        if (origin > 0) {
            --origin;
            pred = node_->parent_->nodeAt(origin);
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
                    otherNode = otherNode->parent_->nodeAt(origin - 1);
                    setOriginAtDepth(otherDepth, origin - 1);
                    for (; otherDepth < depth_; ++otherDepth) {
                        setOriginAtDepth(otherDepth + 1, otherNode->fill_ - 1);
                        otherNode = static_cast<Branch *>(otherNode)->nodeAt(otherNode->fill_ - 1);
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
    if (depth_ > 0 && hasSucc()) {
        assert(node_);
        unsigned origin = getOrigin();
        if (origin + 1 < node_->parent_->fill_) {
            ++origin;
            succ = node_->parent_->nodeAt(origin);
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
                    otherNode = otherNode->parent_->nodeAt(origin + 1);
                    setOriginAtDepth(otherDepth, origin + 1);
                    for (; otherDepth < depth_; ++otherDepth) {
                        setOriginAtDepth(otherDepth + 1, 0);
                        otherNode = static_cast<Branch *>(otherNode)->nodeAt(0);
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

void Tree::updateWeights(const Path *path, Weight delta, unsigned minDepth)
{
    Node *node = path->node();
    for (unsigned i = path->depth(); i > minDepth; --i) {
        Branch *parent = node->parent_;
        assert(parent);
        unsigned origin = path->getOriginAtDepth(i);
        parent->weightAt(origin) += delta;
        node = parent;
    }
}

void Tree::joinSucc(Path *path, Node *newNode)
{
    assert(path);
    assert(newNode->fill_ == 0);

    newNode->succ_ = path->node()->succ_;
    path->node()->succ_ = newNode;

    if (path->hasParent()) {
        Local<Path> parent = path->getParent();
        parent->setNodeIndex(parent->nodeIndex() + 1);
        unsigned heightSaved = height_;
        dissipate<Branch>(&parent);
        if (height_ != heightSaved) path->grow();
        parent->branch()->push(parent->nodeIndex(), Local<Head>{.weight_ = 0, .node_ = newNode});
    }
    else {
        assert(path->node() == rootNode_);
        Branch *branch = new Branch;
        branch->push(0, Local<Head>{.weight_ = rootWeight_, .node_ = rootNode_});
        branch->push(1, Local<Head>{.weight_ = 0, .node_ = newNode});
        rootNode_ = branch;
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
    while (rootNode_ && rootNode_->isBranch_ && rootNode_->fill_ == 1) {
        Branch *branch = static_cast<Branch *>(rootNode_);
        rootNode_ = branch->nodeAt(0);
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
    dotifyNode(format, rootNode_, rootWeight_);
    format << "}\n";
    return format;
}

void Tree::dotifyNode(Format &format, const Node *node, Weight weight, unsigned origin) const
{
    if (node->isBranch_) {
        format
            << "branch_" << (void *)node << " [\n"
            << "label = \"<f0>origin: " << origin << "|<f1>weight: " << weight << "|<f2>fill: " << node->fill_ << "|<f3>succ: " << (void *)node->succ_ << "\"\n"
            << "shape = \"record\"\n"
            << "];\n";

        const Branch *branch = static_cast<const Branch *>(node);
        for (unsigned i = 0; i < branch->fill_; ++i) {
            dotifyNode(format, branch->nodeAt(i), branch->weightAt(i), i);
        }
        for (unsigned i = 0; i < branch->fill_; ++i) {
            Node *child = branch->nodeAt(i);
            format << "branch_" << (void *)node << " -> " << (child->isBranch_ ? "branch_" : "node_") << (void *)child << ":f0;" << nl;
        }
    }
    else {
        format
            << "node_" << (void *)node << " [\n"
            << "label = \"<f0>origin: " << origin << "|<f1>weight: " << weight << "|<f2>fill: " << node->fill_ << "|<f3>succ: " << (void *)node->succ_ << "\"\n"
            << "shape = \"record\"\n"
            << "];\n";

        if (node->succ_)
            format << "node_" << (void *)node << ":f3 -> node_" << (void *)node->succ_ << ";" << nl;
    }
}

}} // namespace cc::index
