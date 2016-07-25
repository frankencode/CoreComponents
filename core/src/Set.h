/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/containers>
#include <cc/OrdinalTree>

namespace cc {

/** \brief %Set data countainer
  */
template<class T>
class Set: public Object
{
public:
    typedef T Item;

    inline static Ref<Set> create() { return new Set; }
    inline static Ref<Set> clone(Set *a) { return new Set(a); }

    inline int count() const { return tree_.weight(); }

    inline bool has(int index) const {
        return 0 <= index && index < count();
    }
    inline const Item &at(int index) const {
        Node *node = 0;
        if (!tree_.lookupByIndex(index, &node))
            CC_ASSERT(false);
        return node->item_;
    }

    /** Return the index of the first item greater or equal _a_
      */
    inline int first(const Item &a) const { return tree_.first(a); }

    /** Return the index of the first item lower or equal _b_
      */
    inline int last(const Item &b) const { return tree_.last(b); }

    /** Insert a new item if no item with the same value exists already.
      * If currentItem is non-null the item with the same value is returned.
      * The function returns true if the new item was inserted successfully.
      */
    inline bool insert(const Item &item, Item *currentItem = 0, int *index = 0)
    {
        bool found = false;
        bool below = true;
        Node *k = tree_.find(item, &found, &below, index);
        if (found) {
            if (currentItem)
                *currentItem = k->item_;
        }
        else {
            tree_.attach(k, new Node(item), below);
        }
        return !found;
    }

    inline bool remove(const Item &item, int *index = 0)
    {
        bool found;
        Node *k = tree_.find(item, &found, 0, index);
        if (found)
            tree_.remove(k);
        return found;
    }

    inline bool removeAt(int index) {
        Node *node = 0;
        if (!tree_.lookupByIndex(index, &node)) return false;
        tree_.remove(node);
        return true;
    }

    inline bool contains(const Item &item)
    {
        bool found = false;
        tree_.find(item, &found);
        return found;
    }

    inline void push(const Item &item)
    {
        bool found = false;
        bool below = true;
        Node *k = tree_.find(item, &found, &below);
        if (found)
            k->item_ = item;
        else
            tree_.attach(k, new Node(item), below);
    }

    inline void pop(Item *item)
    {
        CC_ASSERT(count() > 0);
        Node *k = tree_.min();
        *item = k->item_;
        tree_.remove(k);
    }

    inline Item pop() {
        Item item;
        pop(&item);
        return item;
    }

    inline void clear() { tree_.clear(); }

private:
    typedef OrdinalTree< OrdinalNode<Item> > Tree;
    typedef typename Tree::Node Node;

    Set() {}
    Set(const Set &b): tree_(b.tree_) {}
    inline const Set &operator=(const Set &b) { tree_ = b.tree_; return *this; }

    Tree tree_;
    Item nullItem_;
};

} // namespace cc
