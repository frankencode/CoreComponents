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

template<class T>
class QueueNode
{
public:
    QueueNode(const T &item):
        item_(item),
        next_(0),
        prev_(0)
    {}
    T item_;
    QueueNode *next_;
    QueueNode *prev_;
};

/** \class Queue Queue.h cc/Queue
  * \ingroup container
  * \brief Waiting queue data container
  */
template<class T>
class Queue: public Object
{
public:
    /// Item type
    typedef T Item;

    /** Create a new queue
      * \return new object instance
      */
    inline static Ref<Queue> create(int = 0) {
        return new Queue;
    }

    /** Add a new item to the end of the queue
      * \param item item value
      */
    void pushBack(const T &item, int = 0)
    {
        Node *node = new Node(item);
        if (tail_) {
            tail_->next_ = node;
            node->prev_ = tail_;
            tail_ = node;
        }
        else {
            head_ = node;
            tail_ = node;
        }
        ++length_;
    }

    /** Add a new item to the head of the queue
      * \param item item value
      */
    void pushFront(const T &item, int = 0)
    {
        Node *node = new Node(item);
        if (head_) {
            head_->prev_ = node;
            node->next_ = head_;
            head_ = node;
        }
        else {
            head_ = node;
            tail_ = node;
        }
        ++length_;
    }

    /** Remove an item from the end of the queue
      * \param item optionally return the item value
      * \return item value
      */
    T popBack(T *item)
    {
        CC_ASSERT(length_ > 0);
        T h;
        if (!item) item = &h;
        Node *node = tail_;
        *item = node->item_;
        tail_ = node->prev_;
        if (!tail_) head_ = 0;
        else tail_->next_ = 0;
        delete node;
        --length_;
        return *item;
    }

    /** Remove an item from the head of the queue
      * \param item optionally return the item value
      * \return item value
      */
    T popFront(T *item)
    {
        CC_ASSERT(length_ > 0);
        T h;
        if (!item) item = &h;
        Node *node = head_;
        *item = node->item_;
        head_ = node->next_;
        if (!head_) tail_ = 0;
        else head_->prev_ = 0;
        delete node;
        --length_;
        return *item;
    }

    /** Remove the last item from the queue
      * \return item value
      */
    inline T popBack() {
        T item;
        return popBack(&item);
    }

    /** Remove the first item from the queue
      * \return item value
      */
    inline T popFront() {
        T item;
        return popFront(&item);
    }

    /// \copydoc pushBack()
    inline void push(const T &item, int = 0) { pushBack(item); }

    /// \copydoc popFront(T *)
    inline T pop(T *item) { return popFront(item); }

    /// \copydoc popFront()
    inline T pop() { return popFront(); }

    /// Return number of queued items
    inline int count() const { return length_; }

    /// Return the first item in the queue
    inline T front() const { return head_->item_; }

    /// Return the last item in the queue
    inline T back() const { return tail_->item_; }

    /// Reset the stack to an empty state
    void deplete()
    {
        Node *node = head_;
        while (node) {
            Node *next = node->next_;
            delete node;
            node = next;
        }
        head_ = 0;
        tail_ = 0;
        length_ = 0;
    }

private:
    typedef QueueNode<T> Node;

    Queue():
        head_(0),
        tail_(0),
        length_(0)
    {}

    ~Queue()
    {
        deplete();
    }

    Node *head_;
    Node *tail_;
    int length_;
};

} // namespace cc
