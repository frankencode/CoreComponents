/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/containers>

namespace cc {

template<class T, template<class> class Order = Ascending>
class GenericHeap: public Object, public Order<T>
{
public:
    typedef T Item;

    ~GenericHeap()
    {
        if (bufOwner_)
        {
            delete[] buf_;
            buf_ = 0;
        }
    }

    inline bool isFull() const { return fill_ == size_; }
    inline bool isEmpty() const { return fill_ == 0; }

    inline void push(const T &item)
    {
        CC_ASSERT(fill_ < size_);
        buf_[fill_] = item;
        ++fill_;
        passUpLast();
    }

    inline void pop(T *item)
    {
        CC_ASSERT(fill_ > 0);
        *item = buf_[0];
        --fill_;
        buf_[0] = buf_[fill_];
        passDownFromTop();
    }

    inline T pop() {
        T item;
        pop(&item);
        return item;
    }

    inline T top() { CC_ASSERT(!isEmpty()); return buf_[0]; }

    inline void clear() { fill_ = 0; }

    inline static int parent(int i) { return (i - 1) / 2; }
    inline static int leftChild(int i) { return 2 * i + 1; }
    inline static int rightChild(int i) { return 2 * i + 2; }

    inline void xchg(int i, int j)
    {
        T h = buf_[i];
        buf_[i] = buf_[j];
        buf_[j] = h;
    }

    inline int min(int i, int j, int k)
    {
        int h = Order<T>::below(buf_[i], buf_[j]) ? i : j;
        return Order<T>::below(buf_[h], buf_[k]) ? h : k;
    }

    void passUpLast()
    {
        if (fill_ == 1) return;
        int i = fill_ - 1;
        while (i != 0) {
            int j = parent(i);
            if (!Order<T>::below(buf_[i], buf_[j])) break;
            xchg(i, j);
            i = j;
        }
    }

    void passDownFromTop()
    {
        if (fill_ == 0) return;
        int i = 0;
        while (true) {
            int j, lc, rc;
            lc = leftChild(i);
            rc = rightChild(i);
            if (rc < fill_) {
                j = Order<T>::below(buf_[lc], buf_[i]) ? lc : i;
                j = Order<T>::below(buf_[rc], buf_[j]) ? rc : j;
                if (j == i) break;
                xchg(i, j);
                i = j;
            }
            else if (lc < fill_) {
                if (Order<T>::below(buf_[lc], buf_[i])) xchg(i, lc);
                break;
            }
            else
                break;
        }
    }

protected:
    GenericHeap(int size)
        : fill_(0),
          size_(size),
          bufOwner_(true),
          buf_(new T[size])
    {}

    GenericHeap(T *buf, int size)
        : fill_(0),
          size_(size),
          bufOwner_(false),
          buf_(buf)
    {}

    int fill_;    // current number of elements
    int size_;    // maximal number of elements
    bool bufOwner_;
    T *buf_;    // memory buffer used for storing elements
};

/** \class Heap Heap.h cc/Heap
  */
template<class T>
class Heap: public GenericHeap<T, FlexibleSortOrder>
{
public:
    typedef GenericHeap<T, FlexibleSortOrder> Super;

    inline static Ref<Heap> create(int size, int order = SortOrder::Ascending) {
        return new Heap(size, order);
    }
    inline static Ref<Heap> create(T *buf, int size, int order = SortOrder::Ascending) {
        return new Heap(buf, size, order);
    }

    void reset(int order) {
        Super::clear();
        Super::setSortOrder(order);
    }

private:
    Heap(int size, int order)
        : GenericHeap<T, FlexibleSortOrder>(size)
    {
        Super::setSortOrder(order);
    }
    Heap(T *buf, int size, int order)
        : GenericHeap<T, FlexibleSortOrder>(buf, size)
    {
        Super::setSortOrder(order);
    }
};

template<class T>
class MinHeap: public GenericHeap<T, Ascending>
{
public:
    inline static Ref<MinHeap> create(int size) {
        return new MinHeap(size);
    }
    inline static Ref<MinHeap> create(T *buf, int size) {
        return new MinHeap(buf, size);
    }
private:
    MinHeap(int size): GenericHeap<T, Ascending>(size) {}
    MinHeap(T *buf, int size): GenericHeap<T, Ascending>(buf, size) {}
};

template<class T>
class MaxHeap: public GenericHeap<T, Descending>
{
public:
    inline static Ref<MaxHeap> create(int size) {
        return new MaxHeap(size);
    }
    inline static Ref<MaxHeap> create(T *buf, int size) {
        return new MaxHeap(buf, size);
    }
private:
    MaxHeap(int size): GenericHeap<T, Descending>(size) {}
    MaxHeap(T *buf, int size): GenericHeap<T, Descending>(buf, size) {}
};

} // namespace cc
