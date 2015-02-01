/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_TREE_H
#define FLUX_TREE_H

#include <flux/Object>

namespace flux {

/** \brief Generic tree structure
  */
template<class Node>
class Tree: public Object
{
public:
    ~Tree() { disbandChildren(); }

    inline Node *parent() const { return parent_; }
    inline Node *firstChild() const { return firstChild_; }
    inline Node *lastChild() const { return lastChild_; }
    inline Node *nextSibling() const { return nextSibling_; }
    inline Node *previousSibling() const { return previousSibling_; }

    inline void appendChild(Node *node) { insertChild(node, lastChild_); }

    void insertChild(Node *node, Node *previousSibling = 0);
    void appendAllChildrenOf(Node *node); // HACK?, better: adoptChildrenOf(...)
    void disbandChildren();
    void unlink();

    // iterating leafs
    Node *firstLeaf() const;
    Node *lastLeaf() const;
    Node *nextLeaf() const;
    Node *previousLeaf() const;

    // iterating all nodes
    inline Node *first() const { return firstLeaf(); }
    inline Node *last() const { return lastLeaf(); }
    inline Node *next() const { return (nextSibling_) ? nextSibling_->firstLeaf() : parent(); }

    inline int countChildren() const {
        return (firstChild_) ? firstChild_->countSiblings() : 0;
    }
    inline bool hasChildren() const { return firstChild_; }
    inline bool hasSingleChild() const { return (firstChild_) && (firstChild_ == lastChild_); }

    int countSiblings() const;

private:
    inline Node *me() const { return cast<Node>(const_cast<Tree<Node> *>(this)); }

    Node *parent_;
    Ref<Node> firstChild_;
    Ref<Node> lastChild_;
    Ref<Node> nextSibling_;
    Node *previousSibling_;
};

template<class Node>
void Tree<Node>::insertChild(Node *node, Node *previousSibling)
{
    if (node->parent_)
        unlink();
    if (previousSibling) {
        FLUX_ASSERT(previousSibling->parent_ == this);
        node->nextSibling_ = previousSibling->nextSibling_;
        node->previousSibling_ = previousSibling;
        previousSibling->nextSibling_ = node;
        if (node->nextSibling_)
            node->nextSibling_->previousSibling_ = node;
        else
            lastChild_ = node;
    }
    else {
        if (firstChild_) {
            node->nextSibling_ = firstChild_;
            node->nextSibling_->previousSibling_ = node;
        }
        else {
            node->nextSibling_ = 0;
            lastChild_ = node;
        }
        node->previousSibling_ = 0;
        firstChild_ = node;
    }

    node->parent_ = me();
}

template<class Node>
void Tree<Node>::appendAllChildrenOf(Node *node)
{
    if (!node->firstChild_)
        return;
    if (lastChild_) {
        lastChild_->nextSibling_ = node->firstChild_;
        node->firstChild_->previousSibling_ = lastChild_;
        lastChild_ = node->lastChild_;
    }
    else {
        firstChild_ = node->firstChild_;
        lastChild_ = firstChild_;
    }
    Node *child = node->firstChild_;
    while (child) {
        child->parent_ = me();
        child = child->nextSibling_;
    }
    node->firstChild_ = 0;
    node->lastChild_ = 0;
}

template<class Node>
void Tree<Node>::disbandChildren()
{
    Node *node = lastChild_;
    while (node) {
        Node *next = node->previousSibling_;
        node->disbandChildren();
        node->nextSibling_ = 0;
        node->previousSibling_ = 0;
        node->parent_ = 0;
        node = next;
    }
    lastChild_ = 0;
    firstChild_ = 0;
}

template<class Node>
void Tree<Node>::unlink()
{
    Ref<Node> self = me();
    if (previousSibling_)
        previousSibling_->nextSibling_ = nextSibling_;
    if (nextSibling_)
        nextSibling_->previousSibling_ = previousSibling_;
    if (parent_) {
        if (parent_->firstChild_ == this)
            parent_->firstChild_ = nextSibling_;
        if (parent_->lastChild_ == this)
            parent_->lastChild_ = previousSibling_;
        parent_ = 0;
    }
    self = 0;
}

template<class Node>
Node *Tree<Node>::firstLeaf() const
{
    Node *node = me();
    while (node->firstChild())
        node = node->firstChild();
    return node;
}

template<class Node>
Node *Tree<Node>::lastLeaf() const
{
    Node *node = me();
    while (node->lastChild())
        node = node->lastChild();
    return node;
}

template<class Node>
Node *Tree<Node>::nextLeaf() const
{
    Node *node = me();

    if (node->nextSibling()) {
        node = node->nextSibling()->firstLeaf();
    }
    else {
        while (node->parent()) {
            node = node->parent();
            if (node->nextSibling()) {
                node = node->nextSibling()->firstLeaf();
                break;
            }
        }
        if (!node->parent())
            node = 0;
    }

    return node;
}

template<class Node>
Node *Tree<Node>::previousLeaf() const
{
    Node *node = me();

    if (node->previousSibling()) {
        node = node->previousSibling()->lastLeaf();
    }
    else {
        while (node->parent()) {
            node = node->parent();
            if (node->previousSibling()) {
                node = node->previousSibling()->lastLeaf();
                break;
            }
        }
        if (!node->parent())
            node = 0;
    }

    return node;
}

template<class Node>
int Tree<Node>::countSiblings() const
{
    int n = 0;
    for (const Node *node = me(); node; node = node->nextSibling_) ++n;
    return n;
}

} // namespace flux

#endif // FLUX_TREE_H
