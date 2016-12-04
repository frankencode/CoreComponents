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

/** \class Set Set.h cc/Set
  * \ingroup container
  * \brief %Set data countainer
  */
template<class T>
class Set: public Object
{
public:
    /// Item type
    typedef T Item;

    /** Create a new set
      * \return new object instance
      */
    inline static Ref<Set> create() { return new Set; }

    /** Create a shallow copy of another set
      * \param other another set
      * \return new object instance
      */
    inline static Ref<Set> replicate(const Set *other) { return new Set(other); }

    /// Number of items
    inline int count() const { return tree_.weight(); }

    /// Check if an index is valid
    inline bool has(int index) const {
        return 0 <= index && index < count();
    }

    /** Return a reference to an item
      * \param index set index
      * \return reference to the item
      */
    inline const Item &at(int index) const {
        Node *node = 0;
        if (!tree_.lookupByIndex(index, &node))
            CC_ASSERT(false);
        return node->item_;
    }

    /** Return the index of the first item greater or equal _a_
      */
    inline int from(const Item &a) const { return tree_.first(a); }

    /** Return the index of the first item lower or equal _b_
      */
    inline int to(const Item &b) const { return tree_.last(b); }

    /** Insert a new item if no item with the same value exists already.
      * If currentItem is non-null the item with the same value is returned.
      * \return true if the new item was inserted successfully
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

    /** Remove an item from the set
      * \param item item value
      * \param index returns the index of the item that has been removed
      * \return true if the matching item was found and removed
      */
    inline bool remove(const Item &item, int *index = 0)
    {
        bool found;
        Node *k = tree_.find(item, &found, 0, index);
        if (found)
            tree_.remove(k);
        return found;
    }

    /** Remove an item at a given index
      * \param index set index
      * \return true if the matching item was found and removed
      */
    inline bool removeAt(int index) {
        Node *node = 0;
        if (!tree_.lookupByIndex(index, &node)) return false;
        tree_.remove(node);
        return true;
    }

    /** Check if the set contains a given item
      * \param item item value
      * \return true if the set contains the given item
      */
    inline bool contains(const Item &item)
    {
        bool found = false;
        tree_.find(item, &found);
        return found;
    }

    /** Add a new item to the set
      * \param item new item value
      */
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

    /** Remove first item from the set
      * \param item return the item's value
      */
    inline void pop(Item *item)
    {
        CC_ASSERT(count() > 0);
        Node *k = tree_.min();
        *item = k->item_;
        tree_.remove(k);
    }

    /** Remove first item from the set
      * \return the item's value
      */
    inline Item pop() {
        Item item;
        pop(&item);
        return item;
    }

    /// Reset this set to an empty set
    inline void deplete() { tree_.clear(); }

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
