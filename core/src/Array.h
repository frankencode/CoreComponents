/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/strings>
#include <cc/containers>

namespace cc {

/** \class Array Array.h cc/Array
  * \ingroup container
  * \brief Typeded memory vector
  * \see List, ByteArray
  */
template<class T>
class Array: public Object
{
public:
    /// Item type
    typedef T Item;

    /** Create a new array
      * \param size size of the array
      * \return new object instance
      */
    inline static Ref<Array> create(int size) { return new Array(size); }

    /** Create a new array
      * \param size size of the array
      * \param zero preset item value
      * \return new object instance
      */
    inline static Ref<Array> create(int size, T zero) { return new Array(size, zero); }

    /** Create a deep copy of a memory vector
      * \param data pointer to a memory vector of type T
      * \param size size of the memory vector
      * \return new object instance
      */
    inline static Ref<Array> copy(const T *data, int size) { return new Array(data, size); }

    /** Write contents
      * \param b source to copy
      */
    void write(const Array *b)
    {
        const int size = size_ < b->size_ ? b->size_ : size_;
        memcpy(data_, b->data_, size * sizeof(T));
    }

    /** %Set all items to a default value
      * \param zero default value
      */
    inline void fill(const T &zero)
    {
        for (int i = 0, n = size_; i < n; ++i)
            data_[i] = zero;
    }

    /** Shrink in size
      * \param newSize new size
      */
    inline void truncate(int newSize)
    {
        if (newSize <= 0)
            reset();
        else if (newSize < size_)
            size_ = newSize;
    }

    /// Number of items
    inline int count() const { return size_; }

    /** Check if index is within range
      * \param i item index
      * \return true if i is a valid index
      */
    inline bool has(int i) const {
        return (0 <= i) && (i < size_);
    }

    /** Access character at index i (readonly)
      * \param i byte index
      * \return low-level reference
      */
    inline const T &at(int i) const {
        CC_ASSERT(has(i));
        return data_[i];
    }

    /** Access character at index i
      * \param i byte index
      * \return low-level reference
      */
    inline T &at(int i) {
        CC_ASSERT(has(i));
        return data_[i];
    }

    /// Return byte pointer to the internal memory vector (readonly)
    inline const T *data() const { return data_; }

    /// Return byte pointer to the internal memory vector
    inline T *data() { return data_; }

    /// Cast to low-level C array (readonly)
    inline operator const T*() const { return data_; }

    /// Cast to low-level C array
    inline operator T*() { return data_; }

    /// Create a deep copy of this string
    inline Ref<Array> copy() const { return copy(0, size_); }

    /** Copy a range
      * \param i0 begin of range (index of first selected item)
      * \param i1 end of range (index behind the last selected item)
      * \return copied range
      */
    inline Ref<Array> copy(int i0, int i1) const {
        if (i0 < 0) i0 = 0;
        if (i0 > size_) i0 = size_;
        if (i1 < 0) i1 = 0;
        if (i1 > size_) i1 = size_;
        return (i0 < i1) ? new Array(data_ + i0, i1 - i0) : new Array();
    }

    /** Copy the head of this array
      * \param n size of the head
      * \return copied array
      */
    inline Ref<Array> head(int n) const { return copy(0, n); }


    /** Copy the tail of this string
      * \param n size of the tail
      * \return copied array
      */
    inline Ref<Array> tail(int n) const { return copy(size_ - n, size_); }

    /** Search for an item value
      * \param item item value to search for
      * \param index start position for the search
      * \return position the item value was found in or count() if item value could not be found
      */
    inline int find(const T &item, int index = 0) const {
        while (index < size_)
            if (data_[index++] == item) return index - 1;
        return size_;
    }

    /** Check if the array contains a certain item value
      * \param item item value
      * \return true if item value appears in the array
      */
    inline bool contains(const T &item) const { return find(item) < size_; }

    /** Replace all appearances of an item value
      * \param oldItem old item value to search for
      * \param newItem new item value to put in place
      * \return pointer to this array
      */
    inline Array *replaceInsitu(const T &oldItem, const T &newItem) {
        for (int i = 0; i < size_; ++i) {
            if (data_[i] == oldItem)
                data_[i] = newItem;
        }
        return this;
    }

    /** Find a pattern
      * \param pattern pattern to search for
      * \param i start search from this index
      * \return index of first occurence of pattern or count() if not found
      */
    inline int find(const Array *pattern, int i) const { return find(i, pattern->data(), pattern->count()); }

    /** Find a pattern
      * \param pattern pattern to search for
      * \param patternSize size of the pattern
      * \param i start search from this index
      * \return index of first occurence of pattern or count() if not found
      */
    inline int find(const T *pattern, int patternSize, int i) const {
        if (patternSize == 0) return size_;
        for (int j = i, k = 0; j < size_;) {
            if (data_[j++] == pattern[k]) {
                ++k;
                if (k == patternSize)
                    return j - k;
                k = 0;
            }
        }
        return size_;
    }


    /// Check if this array contains a certain pattern
    inline bool contains(Array *pattern) { return contains(pattern->data(), pattern->count()); }

    /// Check if this array contains a certain pattern
    inline bool contains(const T *pattern, int patternSize) { return find(0, pattern, patternSize) != size_; }

    /** STL-style iterator declarations
      * @{
      */
    typedef Iterator<Array> iterator;
    iterator begin() { return iterator(this, 0); }
    iterator end() { return iterator(this, count()); }
    /** @} */

private:
    Array(int size):
        size_(0),
        data_(0)
    {
        if (size > 0) {
            size_ = size;
            data_ = new T[size];
        }
    }

    Array(int size, T zero):
        size_(0),
        data_(0)
    {
        if (size > 0) {
            size_ = size;
            data_ = new T[size];
            memclr(data_, size * sizeof(T), zero);
        }
    }

    Array(const T *data, int size):
        size_(0),
        data_(0)
    {
        if (size > 0) {
            size_ = size;
            data_ = new T[size];
            memcpy(data_, data, size * sizeof(T));
        }
    }

    Array(const Array &b):
        size_(0),
        data_(0)
    {
        if (b.size_ > 0) {
            size_ = b.size_;
            data_ = new T[b.size_];
            memcpy(data_, b.data_, b.size_ * sizeof(T));
        }
    }

    ~Array()
    {
        if (size_ > 0) delete[] data_;
    }

    inline void reset(int newSize = 0)
    {
        if (size_ != newSize) {
            if (size_ > 0) delete[] data_;
            if (newSize > 0) {
                size_ = newSize;
                data_ = new T[newSize];
            }
            else {
                size_ = 0;
                data_ = 0;
            }
        }
    }

    int size_;
    T *data_;
};

template<class T>
bool operator==(const Array<T> &a, const Array<T> &b) { return container::compare(a, b) == 0; }

template<class T>
bool operator!=(const Array<T> &a, const Array<T> &b) { return container::compare(a, b) != 0; }

template<class T>
bool operator<(const Array<T> &a, const Array<T> &b) { return container::compare(a, b) < 0; }

template<class T>
bool operator>(const Array<T> &a, const Array<T> &b) { return container::compare(a, b) > 0; }

template<class T>
bool operator<=(const Array<T> &a, const Array<T> &b) { return container::compare(a, b) <= 0; }

template<class T>
bool operator>=(const Array<T> &a, const Array<T> &b) { return container::compare(a, b) >= 0; }

} // namespace cc
