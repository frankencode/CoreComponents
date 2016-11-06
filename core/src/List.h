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
#include <cc/Heap>

namespace cc {

/** \class List List.h cc/List
  * \brief %List data container
  */
template<class T>
class List: public Object
{
public:
    /// Item type
    typedef T Item;

    /** Create a new empty list
      * \return new object instance
      */
    inline static Ref<List> create() { return new List; }

    /** Create a new list a populate it with n default items
      * \return new object instance
      */
    inline static Ref<List> create(int n) { return new List(n); }

    /** Create a deep copy of another list
      * \param other the other list
      * \return new object instance
      */
    static Ref<List> copy(const List *other)
    {
        Ref<List> newList = List::create(other->count());
        for (int i = 0; i < newList->count(); ++i) newList->at(i) = T::copy(other->at(i));
        return newList;
    }

    /** Create a shallow copy of another list
      * \param other the other list
      * \return new object instance
      */
    inline static Ref<List> clone(const List *other) { return new List(*other); }

    /// Return the number of items stored in this list
    inline int count() const { return tree_.weight(); }

    /// Check if an index is valid
    inline bool has(int index) const {
        return 0 <= index && index < count();
    }

    /** Return a reference to an item
      * \param index list index
      * \return reference to the item
      */
    inline Item &at(int index) const {
        Node *node = 0;
        if (!tree_.lookupByIndex(index, &node))
            CC_ASSERT(false);
        return node->item_;
    }

    /// Return a reference to the first item stored in the list
    inline Item &first() const { return at(0); }

    /// Return a reference to the last item stored in the list
    inline Item &last() const { return at(count() - 1); }

    /** Insert a new item at a certain position
      * \param index list index
      * \param item new item value
      * \return reference to this list
      */
    inline List &push(int index, const Item &item) {
        tree_.push(index, item);
        return *this;
    }

    /** Remove an item from a certain position
      * \param index list index
      * \param item returns the item value
      * \return reference to this list
      */
    inline List &pop(int index, Item *item) {
        tree_.pop(index, item);
        return *this;
    }

    /// Reset this list to an empty list
    inline void clear() { tree_.clear(); }

    /** Remove an item from a certain position
      * \param index list index
      * \return item value
      */
    inline Item pop(int index) {
        Item item;
        pop(index, &item);
        return item;
    }

    /** Append a new item to the end of the list
      * \param item new item value
      */
    inline void push(const Item &item) { push(count(), item); }

    /** Remove an item from the head of the list
      * \param item returns the item value
      */
    inline void pop(Item *item) { pop(0, item); }

    /** Remove an item from the head of the list
      * \return item value
      */
    inline Item pop() { Item item; pop(&item); return item; }

    /** Append a new item to the end of the list
      * \param item new item value
      */
    inline void append(const Item &item) { push(count(), item); }

    /** Append another list to this list
      * \param b another list
      */
    inline void appendList(const List *b) { if (b) for (int i = 0; i < b->count(); ++i) append(b->at(i)); }

    /** Insert a new item at a certain position
      * \param index list index#
      * \param item new item value
      */
    inline void insert(int index, const Item &item) { push(index, item); }

    /** Remove an item from a certain position
      * \param index list index
      * \param item returns the item value
      */
    inline void remove(int index, Item *item) { pop(index, item); }

    /** Remove an item at a given position
      * \param index list index
      */
    inline void remove(int index) { pop(index); }

    /** Insert a new item at head of the list
      * \param item new item
      */
    inline void pushFront(const Item &item) { push(0, item); }

    /** Append a new item to the end of the list
      * \param item new item value
      */
    inline void pushBack(const Item &item) { push(count(), item); }

    /** Remove an item from the head of the list
      * \return item value
      */
    inline Item popFront() { return pop(0); }

    /** Remove an item from the end of the list
      * \return item value
      */
    inline Item popBack() { return pop(count() - 1); }

    /** Search for an item value
      * \param item item value to search for
      * \param index start position for the search
      * \return position the item value was found in or count() if item value could not be found
      */
    int find(const Item &item, int index = 0) const
    {
        while (index < count()) {
            if (at(index) == item) break;
            ++index;
        }
        return index;
    }

    /** Check if the list contains a certain item value
      * \param item item value
      * \return true if item value appears in the list
      */
    inline bool contains(const Item &item) const { return find(item) < count(); }

    /** Replace all appearances of an item value
      * \param oldItem old item value to search for
      * \param newItem new item value to put in place
      * \return pointer to this list
      */
    List *replaceInsitu(const Item &oldItem, const Item &newItem)
    {
        for (int i = 0; i < count(); ++i) {
            if (at(i) == oldItem)
                at(i) = newItem;
        }
        return this;
    }

    /** Join all items of this list into a single item
      * \param sep the separator to insert between the items
      * \return the resulting item
      */
    inline Item join(const Item &sep = Item()) const { return Item::join(this, sep); }

    /** Create a sorted copy of this list
      * \param order sort order
      * \param unique eliminate duplicates
      * \return newly sorted list
      */
    Ref<List> sort(SortOrder order = Ascending, bool unique = false) const
    {
        if (count() == 0)
            return List::create();
        Ref< Heap<Item> > heap = Heap<Item>::create(count(), order);
        for (int i = 0; i < count(); ++i)
            heap->push(at(i));
        Ref<List> result;
        if (unique) {
            result = List::create();
            Item prev, item;
            heap->pop(&prev);
            result->append(prev);
            while (!heap->isEmpty()) {
                heap->pop(&item);
                if (item != prev) {
                    result->append(item);
                    prev = item;
                }
            }
        }
        else {
            result = List::create(count());
            for (int i = 0, n = result->count(); i < n; ++i)
                result->at(i) = heap->pop();
        }
        return result;
    }

    /** Create a reversed copy of this list
      * return reversed list
      */
    Ref<List> reverse() const
    {
        Ref<List> result = List::create(count());
        for (int i = 0, n = count(); i < n; ++i)
            result->at(i) = at(n - i - 1);
        return result;
    }

    /** Create a sorted copy of this list with duplicates eliminated
      * \param order sort order
      * \return newly sorted list
      */
    Ref<List> unique(SortOrder order = Ascending) const { return sort(order, true); }

    /** Convert to a list of different item type
      * \tparam T2 the target item type
      * \return new list instance
      */
    template<class T2>
    Ref< List<T2> > toList() const {
        Ref< List<T2 > > result = List<T2>::create(count());
        for (int i = 0; i < count(); ++i)
            result->at(i) = at(i);
        return result;
    }

    /// \copydoc push(const Item &item)
    inline void operator<<(const T& item) { push(item); }

    /// \copydoc pop()
    inline void operator>>(T* item) { pop(item); }

private:
    typedef OrdinalTree< OrdinalNode<Item> > Tree;
    typedef typename Tree::Node Node;

    List() {}
    List(int n): tree_(n) {}

    explicit List(const List &b): tree_(b.tree_) {}
    const List &operator=(const List &b);

    Tree tree_;
    mutable Item nullItem_;
};

template<class T>
bool operator==(const List<T> &a, const List<T> &b) { return container::compare(a, b) == 0; }

template<class T>
bool operator!=(const List<T> &a, const List<T> &b) { return container::compare(a, b) != 0; }

template<class T>
bool operator<(const List<T> &a, const List<T> &b) { return container::compare(a, b) < 0; }

template<class T>
bool operator>(const List<T> &a, const List<T> &b) { return container::compare(a, b) > 0; }

template<class T>
bool operator<=(const List<T> &a, const List<T> &b) { return container::compare(a, b) <= 0; }

template<class T>
bool operator>=(const List<T> &a, const List<T> &b) { return container::compare(a, b) >= 0; }

} // namespace cc
