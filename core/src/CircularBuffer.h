/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/assert>
#include <cc/Array>

namespace cc {

/** \class CircularBuffer CircularBuffer.h cc/CircularBuffer
  * \brief fixed size queue
  */
template<class T>
class CircularBuffer: public Object
{
public:
    typedef T Item;

    inline static Ref<CircularBuffer> create(int size) {
        return new CircularBuffer(size);
    }

    inline int size() const { return size_; }
    inline int count() const { return fill_; }
    inline bool isFull() const { return fill_ == size_; }
    inline bool isEmpty() const { return fill_ == 0; }

    inline bool has(int i) const { return (0 <= i) && (i < fill_); }
    inline T& at(int i) const { return front(i); }
    inline T get(int i) const { return front(i); }

    inline CircularBuffer& push(const T& item) { return pushBack(item); }
    inline CircularBuffer& pop(T* item) { return popFront(item); }
    inline T pop() { T item; popFront(&item); return item; }

    inline void clear()
    {
        fill_ = 0;
        head_ = size_ - 1;
        tail_ = size_ - 1;
    }

    inline CircularBuffer& pushBack(const T& item)
    {
        CC_ASSERT(fill_ != size_);
        ++head_;
        if (head_ >= size_) head_ = 0;
        ++fill_;
        buf_->at(head_) = item;
        return *this;
    }

    inline CircularBuffer& popFront(T* item)
    {
        CC_ASSERT(fill_ > 0);
        ++tail_;
        if (tail_ >= size_) tail_ = 0;
        --fill_;
        *item = buf_->at(tail_);
        return *this;
    }

    inline CircularBuffer& pushFront(const T& item)
    {
        CC_ASSERT(fill_ < size_);
        buf_->at(tail_) = item;
        --tail_;
        if (tail_ < 0) tail_ = size_ - 1;
        ++fill_;
        return *this;
    }

    inline CircularBuffer& popBack(T* item)
    {
        CC_ASSERT(fill_ > 0);
        *item = buf_->at(head_);
        --head_;
        if (head_ < 0) head_ = size_ - 1;
        --fill_;
        return *this;
    }

    inline T popFront() { T item; popFront(&item); return item; }
    inline T popBack() { T item; popBack(&item); return item; }

    inline T& back(int i = 0) const
    {
        CC_ASSERT((0 <= i) && (i < fill_));
        i = -i;
        i += head_;
        if (i < 0) i += size_;
        return buf_->at(i);
    }

    inline T& front(int i = 0) const
    {
        CC_ASSERT((0 <= i) && (i < fill_));
        i += tail_ + 1;
        if (i >= size_) i -= size_;
        return buf_->at(i);
    }

private:
    CircularBuffer(int size):
        fill_(0),
        size_(size),
        head_(size_-1),
        tail_(size_-1),
        buf_(Array<T>::create(size_))
    {}

    ~CircularBuffer()
    {}

    int fill_;
    int size_;
    int head_;
    int tail_;
    Ref< Array<T> > buf_;
};

} // namespace cc
