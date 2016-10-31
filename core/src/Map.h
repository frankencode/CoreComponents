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

/** \class Map Map.h cc/Map
  * \brief %Map data container
  * \see Set, List
  */
template<class Key, class Value = Key>
class Map: public Object
{
public:
    typedef Pair<Key,Value> Item;

    inline static Ref<Map> create() { return new Map(); }
    inline static Ref<Map> clone(Map *a) { return new Map(*a); }

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

    inline const Key &keyAt(int index) const { return at(index).key(); }
    inline const Value &valueAt(int index) const { return const_cast<Item &>(at(index)).value(); }
    inline Value &valueAt(int index) { return const_cast<Item &>(at(index)).value(); }

    /** Return the index of the first item greater or equal _a_
      */
    inline int first(const Key &a) const { return tree_.first(a); }

    /** Return the index of the first item lower or equal _b_
      */
    inline int last(const Key &b) const { return tree_.last(b); }

    /** Insert a key-value mapping if no key-value mapping with the same key exists already.
      * If currentValue is non-null the current value the giving key maps to is returned.
      * The function returns true if the new key-value mapping was inserted successfully.
      */
    inline bool insert(const Key &key, const Value &value, Value *currentValue = 0, int *index = 0)
    {
        bool found = false;
        bool below = true;
        Item e(key, value);
        Node *k = tree_.find(e, &found, &below, index);
        if (found) {
            if (currentValue)
                *currentValue = k->item_.value();
        }
        else {
            tree_.attach(k, new Node(e), below);
        }
        return !found;
    }

    inline bool remove(const Key &key, int *index = 0)
    {
        bool found;
        Node *k = tree_.find(Item(key), &found, 0, index);
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

    /** Insert or overwrite a key-value mapping.
      */
    inline void establish(const Key &key, const Value &value) {
        int index = 0;
        if (!insert(key, value, 0, &index))
            valueAt(index) = value;
    }

    /** Lookup key-value pair by given key.
      * If a matching key-value pair is found the value is returned in 'value' and the
      * function returns with true. If a matching key-value pair could not be found
      * 'value' is not set and the function returns false.
      */
    template<class Value2>
    inline bool lookup(const Key &key, Value2 *value = 0, int *index = 0) const
    {
        bool found = false;
        Node *k = tree_.find(Item(key), &found, 0, index);
        if (found && (value))
            *value = k->item_.value();
        return found;
    }

    /** Retrieve value of an existing key-value mapping.
      */
    inline Value value(const Key &key, const Value &fallback = Value()) const
    {
        Value value = fallback;
        lookup(key, &value);
        return value;
    }

    /** Set value of an existing key-value mapping.
      */
    inline bool setValue(const Key &key, const Value &value)
    {
        int index = 0;
        bool found = lookup(key, (Value *)0, &index);
        if (found)
            valueAt(index) = value;
        return found;
    }

    /** Convenience wrapper to lookup()
      */
    inline bool contains(const Key &key) const { return lookup<Value>(key); }

    inline void push(const Item &item)
    {
        establish(item.key(), item.value());
    }

    inline void pop(Item *item)
    {
        CC_ASSERT(count() > 0);
        Node *k = static_cast<Node *>(tree_.min());
        *item = k->item_;
        tree_.remove(k);
    }

    inline Item pop() {
        Item item;
        pop(&item);
        return item;
    }

    inline void clear() { tree_.clear(); }

    inline void operator<<(const Item &item) { push(item); }

protected:
    typedef OrdinalTree< OrdinalNode<Item> > Tree;
    typedef typename Tree::Node Node;

    Map() {}
    explicit Map(const Map &b): tree_(b.tree_) {}

    const Map &operator=(const Map &b);

    Tree tree_;
    mutable Item nullItem_;
};

#if __cplusplus >= 201103L

template<class Key, class Value>
class ConstMapIterator {
public:
    ConstMapIterator(const Map<Key, Value> *m, int i):
        m_(m),
        i_(i)
    {}

    inline const ConstMapIterator *operator*() const { return this; }

    inline const Key &key() const { return m_->keyAt(i_); }
    inline const Value &value() const { return m_->valueAt(i_); }
    inline int index() const { return i_; }

    inline ConstMapIterator &operator++() { ++i_; return *this; }

private:
    template<class Key2, class Value2>
    friend bool operator!=(const ConstMapIterator<Key2, Value2> &a, const ConstMapIterator<Key2, Value2> &b);

    const Map<Key, Value> *m_;
    int i_;
};

template<class Key, class Value>
inline bool operator!=(const ConstMapIterator<Key, Value> &a, const ConstMapIterator<Key, Value> &b)
{
    return a.i_ != b.i_;
}

template<class Key, class Value>
inline ConstMapIterator<Key, Value> begin(const Map<Key, Value> *m) { return ConstMapIterator<Key, Value>(m, 0); }

template<class Key, class Value>
inline ConstMapIterator<Key, Value> end(const Map<Key, Value> *m) { return ConstMapIterator<Key, Value>(m, m->count()); }

template<class Key, class Value>
inline ConstMapIterator<Key, Value> begin(Ref< const Map<Key, Value> > &m) { return ConstMapIterator<Key, Value>(m, 0); }

template<class Key, class Value>
inline ConstMapIterator<Key, Value> end(Ref< const Map<Key, Value> > &m) { return ConstMapIterator<Key, Value>(m, m->count()); }

template<class Key, class Value>
class MapIterator {
public:
    MapIterator(Map<Key, Value> *m, int i):
        m_(m),
        i_(i)
    {}

    inline MapIterator *operator*() { return this; }

    inline const Key &key() const { return m_->keyAt(i_); }
    inline const Value &value() const { return m_->valueAt(i_); }
    inline Value &value() { return m_->valueAt(i_); }
    inline int index() const { return i_; }

    inline MapIterator &operator++() { ++i_; return *this; }

private:
    template<class Key2, class Value2>
    friend bool operator!=(const MapIterator<Key2, Value2> &a, const MapIterator<Key2, Value2> &b);

    Map<Key, Value> *m_;
    int i_;
};

template<class Key, class Value>
inline bool operator!=(const MapIterator<Key, Value> &a, const MapIterator<Key, Value> &b)
{
    return a.i_ != b.i_;
}

template<class Key, class Value>
inline MapIterator<Key, Value> begin(Map<Key, Value> *m) { return MapIterator<Key, Value>(m, 0); }

template<class Key, class Value>
inline MapIterator<Key, Value> end(Map<Key, Value> *m) { return MapIterator<Key, Value>(m, m->count()); }

template<class Key, class Value>
inline MapIterator<Key, Value> begin(Ref< Map<Key, Value> > &m) { return MapIterator<Key, Value>(m, 0); }

template<class Key, class Value>
inline MapIterator<Key, Value> end(Ref< Map<Key, Value> > &m) { return MapIterator<Key, Value>(m, m->count()); }

#endif // __cplusplus >= 201103L

} // namespace cc
