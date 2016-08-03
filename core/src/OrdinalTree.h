/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/assert>
#include <cc/AvlBalance>
#include <cc/BinaryTree>
#include <cc/ExclusiveAccess>
#include <cc/Array>

namespace cc {

template<class ItemType>
class OrdinalNode: public BinaryNode
{
public:
    typedef ItemType Item;

    OrdinalNode()
        : item_(Item())
    {}
    OrdinalNode(const Item &item)
        : item_(item)
    {}
    OrdinalNode(const OrdinalNode &kb)
        : BinaryNode(kb),
          item_(kb.item_)
    {}

    inline OrdinalNode *left() const { return static_cast<OrdinalNode *>(left_); };
    inline OrdinalNode *right() const { return static_cast<OrdinalNode *>(right_); }
    inline OrdinalNode *parent() const { return static_cast<OrdinalNode *>(parent_); }
    Item item_;
};

template<class NodeType>
class OrdinalTree: public BinaryTree, public AvlBalance
{
public:
    typedef NodeType Node;
    typedef typename NodeType::Item Item;

    OrdinalTree(int n = 0);
    ~OrdinalTree() { clear(); }

    OrdinalTree(const OrdinalTree &b);
    const OrdinalTree &operator=(const OrdinalTree &b);

    bool lookupByIndex(int index, Node **node = 0) const;

    template<class Pattern>
    Node *find(const Pattern &pattern, bool *found = 0, bool *below = 0, int *index = 0) const;

    int first(const Item &a) const;
    int last(const Item &b) const;

    inline int weight() const { return BinaryTree::weight(root_); }

    inline Node *min() const { return static_cast<Node *>(BinaryTree::min(root_)); }
    inline Node *max() const { return static_cast<Node *>(BinaryTree::max(root_)); }

    void push(int index, const Item &item);
    void pop(int index, Item *item);

    inline void remove(Node *k) { return delete static_cast<Node *>(BinaryTree::unlink(k)); }

    static Node *clone(Node *k);
    static void clear(Node *k);
    void clear();

protected:
    inline void setRoot(BinaryNode *k) { root_ = static_cast<Node *>(k); }

    void changed(BinaryNode *kp, BinaryNode *kc, bool left, bool attached);
    void rotated(BinaryNode *k1, bool left);

#ifndef NDEBUG
    static bool testStructure(BinaryNode *k);
    static bool testWeight(BinaryNode *k);
    static bool testIteration(BinaryNode *k);
    static bool testOrder(Node *k);
#endif

    mutable ExclusiveSection cacheExclusive_;
    mutable Node *cachedNode_;
    mutable int cachedIndex_;
    Node *root_;
};

template<class Node>
OrdinalTree<Node>::OrdinalTree(int n)
    : cachedNode_(0),
      root_(0)
{
    if (n <= 0) return;

    Ref< Array<Node *> > v = Array<Node *>::create(n);

    for (int i = 0, m = 1; i < n; m *= 2)
    {
        for (int i1 = i + m; i < i1; ++i)
        {
            if (i < n) {
                Node *k = new Node;
                Node *kp = 0;
                if (i > 0) kp = v->at((i - 1) >> 1);
                k->parent_ = kp;
                if (kp) {
                    if (i & 1)
                        kp->left_ = k;
                    else
                        kp->right_ = k;
                }
                k->left_ = 0;
                k->right_ = 0;
                v->at(i) = k;
            }
            else {
                AvlBalance::restore(v->at((i - 1) >> 1), 0, i & 1, false);
            }
        }
    }

    for (int i = n - 1; i > 0; --i) {
        Node *k = v->at(i);
        Node *kp = k->parent();
        kp->weight_ += k->weight_;
    }

    root_ = v->at(0);

    /*CC_ASSERT(testStructure(this->root_));
    CC_ASSERT(testIteration(this->root_));
    CC_ASSERT(Balance::testBalance(this->root_));
    CC_ASSERT(OrdinalTree<Node>::testWeight(this->root_));*/
}

template<class Node>
OrdinalTree<Node>::OrdinalTree(const OrdinalTree &b)
    : cachedNode_(0)
{
    root_ = clone(b.root_);
}

template<class Node>
const OrdinalTree<Node> &OrdinalTree<Node>::operator=(const OrdinalTree &b)
{
    clear();
    root_ = clone(b.root_);
    return *this;
}

template<class Node>
bool OrdinalTree<Node>::lookupByIndex(int i, Node **node) const
{
    CC_ASSERT((0 <= i) && (i < weight()));

    ExclusiveAccess cacheAccess(&cacheExclusive_);
    if (cacheAccess) {
        if (cachedNode_) {
            const int d = i - cachedIndex_;
            if (d == 1) {
                ++cachedIndex_;
                cachedNode_ = static_cast<Node *>(BinaryTree::succ(cachedNode_));
                if ((cachedNode_) && (node)) *node = cachedNode_;
                return cachedNode_;
            }
            else if (d == 0) {
                if (node) *node = cachedNode_;
                return cachedNode_;
            }
            else if (d == -1) {
                --cachedIndex_;
                cachedNode_ = static_cast<Node *>(BinaryTree::pred(cachedNode_));
                if ((cachedNode_) && (node)) *node = cachedNode_;
                return cachedNode_;
            }
        }
    }

    Node *k = root_;
    int j0 = 0;
    while (k) {
        int j = j0 + BinaryTree::weight(k->left_);
        if (i < j) {
            k = k->left();
        }
        else if (j < i) {
            j0 = j + 1;
            k = k->right();
        }
        else // i == j
            break;
    }
    if ((k) && (node)) *node = k;

    if (cacheAccess) {
        cachedNode_ = k;
        cachedIndex_ = i;
    }

    return k;
}

template<class Node>
template<class Pattern>
Node *OrdinalTree<Node>::find(const Pattern &pattern, bool *found, bool *below, int *index) const
{
    Node *k = root_;
    Node *k2 = 0;
    if (found) *found = false;
    int j0 = 0, j = -1;
    if (k) {
        while (true) {
            k2 = k;
            j = j0 + BinaryTree::weight(k->left_);
            if (pattern < k->item_) {
                if (!k->left_) {
                    if (below) *below = true;
                    break;
                }
                k = k->left();
            }
            else if (k->item_ < pattern) {
                if (!k->right_) {
                    if (below) *below = false;
                    break;
                }
                j0 = j + 1;
                k = k->right();
            }
            else { // item == k->data()
                if (found) *found = true;
                break;
            }
        }
    }
    if (index) *index = j;
    return k2;
}

template<class Node>
inline int OrdinalTree<Node>::first(const Item &a) const
{
    bool found = false, below = true;
    int index = 0;
    if (!root_) return 0;
    find(a, &found, &below, &index);
    if (found) return index;
    return below ? index : index + 1;
}

template<class Node>
inline int OrdinalTree<Node>::last(const Item &b) const
{
    bool found = false, below = true;
    int index = 0;
    if (!root_) return 0;
    find(b, &found, &below, &index);
    if (found) return index;
    return below ? index - 1 : index;
}

template<class Node>
void OrdinalTree<Node>::push(int index, const Item &item)
{
    Node *kn = new Node(item);
    if (index == weight()) {
        Node *kp = 0;
        if (cachedNode_) {
            if (cachedIndex_ == index)
                kp = cachedNode_;
        }
        if (!kp) kp = max();
        BinaryTree::attach(kp, kn, false);
    }
    else {
        Node *ka = 0;
        if (!lookupByIndex(index, &ka))
            CC_ASSERT(false);
        BinaryTree::attachBefore(ka, kn);
    }

    cachedNode_ = kn;
    cachedIndex_ = index;
}

template<class Node>
void OrdinalTree<Node>::pop(int index, Item *item)
{
    Node *ko = 0;
    if (!lookupByIndex(index, &ko))
        CC_ASSERT(false);
    *item = ko->item_;
    Node *k = static_cast<Node *>(BinaryTree::pred(ko));
    if (k) --index;
    else k = static_cast<Node *>(BinaryTree::succ(ko));
    remove(ko);
    if (k) {
        cachedNode_ = k;
        cachedIndex_ = index;
    }
}

template<class Node>
inline void OrdinalTree<Node>::changed(BinaryNode *kp, BinaryNode *kc, bool left, bool attached)
{
    int delta = attached ? 1 : -1;
    BinaryNode *k = kp;
    while (k) {
        k->weight_ += delta;
        k = k->parent_;
    }

    AvlBalance::restore(kp, kc, left, attached);

    cachedNode_ = 0;
}

template<class Node>
inline void OrdinalTree<Node>::rotated(BinaryNode *k1, bool /*left*/)
{
    BinaryTree::establishWeight(k1);
    BinaryTree::establishWeight(k1->parent_);
}

template<class Node>
Node *OrdinalTree<Node>::clone(Node *k)
{
    if (!k) return 0;
    Node *kn = new Node(*k);
    if (!k->parent_) kn->parent_ = 0;
    if (k->left_) {
        kn->left_ = clone(k->left());
        kn->left_->parent_ = kn;
    }
    else {
        kn->left_ = 0;
    }
    if (k->right_) {
        kn->right_ = clone(k->right());
        kn->right_->parent_ = kn;
    }
    else {
        kn->right_ = 0;
    }
    return kn;
}

template<class Node>
void OrdinalTree<Node>::clear()
{
    clear(root_);
    root_ = 0;
    cachedNode_ = 0;
}

template<class Node>
void OrdinalTree<Node>::clear(Node *k)
{
    if (!k) return;
    clear(k->left());
    clear(k->right());
    delete k;
}

#ifndef NDEBUG

template<class Node>
bool OrdinalTree<Node>::testStructure(BinaryNode *k)
{
    if (!k) return true;
    if (k->parent_) {
        if (!((k == k->parent_->left_) || (k == k->parent_->right_)))
            return false;
    }
    return testStructure(k->left_) && testStructure(k->right_);
}

template<class Node>
bool OrdinalTree<Node>::testWeight(BinaryNode *k)
{
    if (!k) return true;
    return
        (weight(k->left_) + weight(k->right_) + 1 == k->weight_) &&
        testWeight(k->left_) && testWeight(k->right_);
}

template<class Node>
bool OrdinalTree<Node>::testIteration(BinaryNode *k)
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

template<class Node>
bool OrdinalTree<Node>::testOrder(Node *k)
{
    if (!k) return true;
    if (k->left_) {
        if (!(k->left()->item_ < k->item_))
            return false;
    }
    if (k->right_) {
        if (!(k->item_ < k->right()->item_))
            return false;
    }
    return testOrder(k->left()) && testOrder(k->right());
}

#endif // ndef NDEBUG

} // namespace cc
