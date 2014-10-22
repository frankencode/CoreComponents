/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_BYTEARRAY_H
#define FLUX_BYTEARRAY_H

#include <flux/containers>
#include <flux/strings>

namespace flux {

class String;
template<class T> class List;
typedef List<String> StringList;

class SyntaxDefinition;

class File;
class ThreadFactory;

template<class SubClass> class Singleton;

class ByteArray: public Object
{
public:
    typedef char Item;

    inline static Ref<ByteArray> create(int size = 0) { return new ByteArray(0, size, Terminated); }
    inline static Ref<ByteArray> create(int size, char zero) {
        Ref<ByteArray> newArray = new ByteArray(0, size, Terminated);
        newArray->clear(zero);
        return newArray;
    }
    inline static Ref<ByteArray> copy(const char *data, int size = -1) { return new ByteArray(data, size, Terminated); }

    inline static Ref<ByteArray> allocate(int size) { return new ByteArray(0, size, Unterminated); }

    static Ref<ByteArray> join(const StringList *parts, const char *sep = "");
    static Ref<ByteArray> join(const StringList *parts, char sep);
    static Ref<ByteArray> join(const StringList *parts, String sep);

    ~ByteArray();

    void resize(int newSize);

    ByteArray &operator=(const ByteArray &b);
    ByteArray &operator^=(const ByteArray &b);

    void clear(char zero = '\0');

    inline int count() const { return size_; }

    inline int first() const { return 0; }
    inline int last() const { return size_ - 1; }

    inline bool has(int i) const {
        return (0 <= i) && (i < size_);
    }

    inline char *pointerAt(int i) const {
        FLUX_ASSERT(has(i));
        return data_ + i;
    }

    inline char &at(int i) const {
        FLUX_ASSERT(has(i));
        return data_[i];
    }
    inline uint8_t &byteAt(int i) const {
        FLUX_ASSERT(has(i));
        return bytes_[i];
    }
    inline uint32_t &wordAt(int j) const {
        FLUX_ASSERT(has(j * 4) && has(j * 4 + 3));
        return words_[j];
    }

    inline char *chars() const {
        FLUX_ASSERT2(flags_ & Terminated, "ByteArray is not terminated by zero and therefore cannot safely be converted to a C string");
        return chars_;
    }
    inline uint8_t *bytes() const { return bytes_; }
    inline uint32_t *words() const { return words_; }
    inline operator char*() const { return chars(); }

    inline Ref<ByteArray> copy() const { return new ByteArray(*this); }

    inline Ref<ByteArray> copy(int i0, int i1) const {
        if (i0 < 0) i0 = 0;
        if (i0 > size_) i0 = size_;
        if (i1 < 0) i1 = 0;
        if (i1 > size_) i1 = size_;
        return (i0 < i1) ? new ByteArray(data_ + i0, i1 - i0) : new ByteArray();
    }

    template<class Range>
    inline Ref<ByteArray> copy(Range *range) const {
        if (!range) return new ByteArray();
        return copy(range->i0(), range->i1());
    }

    Ref<ByteArray> paste(int i0, int i1, String text) const;

    inline Ref<ByteArray> head(int n) const { return copy(0, n); }
    inline Ref<ByteArray> tail(int n) const { return copy(size_ - n, size_); }

    inline int find(char ch) const { return find(ch, 0); }
    inline int find(char ch, int i) const {
        if (i < 0) i = 0;
        while (i < size_) {
            if (data_[i] == ch) break;
            ++i;
        }
        return i;
    }

    inline bool contains(char ch) const { return find(ch) < size_; }
    inline int count(char ch) const {
        int n = 0;
        for (const char *p = data_; *p; ++p) n += (*p == ch);
        return n;
    }
    int count(const char *set);

    inline int replace(char oldItem, char newItem) {
        int n = 0;
        for (int i = 0; i < size_; ++i) {
            if (data_[i] == oldItem) {
                data_[i] = newItem;
                ++n;
            }
        }
        return n;
    }

    int find(const char *pattern, int i = 0) const;
    int find(String pattern, int i = 0) const;
    int find(SyntaxDefinition *pattern, int i = 0) const;

    inline bool contains(const char *pattern) const { return find(pattern) != size_; }
    bool contains(String pattern) const;

    Ref<StringList> split(char sep) const;
    Ref<StringList> split(const char *sep) const;
    Ref<StringList> split(SyntaxDefinition *pattern) const;
    Ref<StringList> breakUp(int chunkSize) const;

    ByteArray *replaceInsitu(const char *pattern, const char *replacement);
    Ref<ByteArray> replace(const char *pattern, const char *replacement) const;
    Ref<ByteArray> replace(const char *pattern, String replacement) const;
    Ref<ByteArray> replace(String pattern, String replacement) const;

    int toInt(bool *ok = 0) const;
    double toFloat(bool *ok = 0) const;
    int64_t toInt64(bool *ok = 0) const;
    uint64_t toUInt64(bool *ok = 0) const;
    float64_t toFloat64(bool *ok = 0) const;

    int scanString(String *x, const char *termination = " \t\n", int i0 = 0, int i1 = -1) const;

    template<class T>
    int scanInt(T *x, int base = 10, int i0 = 0, int i1 = -1) const;

    inline Ref<ByteArray> downcase() const { return copy()->downcaseInsitu(); }
    inline Ref<ByteArray> upcase() const { return copy()->upcaseInsitu(); }
    ByteArray *downcaseInsitu();
    ByteArray *upcaseInsitu();

    Ref<ByteArray> escape() const;
    inline Ref<ByteArray> unescape() const { return copy()->unescapeInsitu(); }
    ByteArray *unescapeInsitu();

    ByteArray *truncate(int newSize);

    inline Ref<ByteArray> trim(const char *leadingSpace = " \t\n\r", const char *trailingSpace = 0) const { return copy()->trimInsitu(leadingSpace, trailingSpace); }
    inline Ref<ByteArray> trimLeading(const char *space = " \t\n\r") const { return copy()->trimInsitu(space, ""); }
    inline Ref<ByteArray> trimTrailing(const char *space = " \t\n\r") const { return copy()->trimInsitu("", space); }
    ByteArray *trimInsitu(const char *leadingSpace = " \t\n\r", const char *trailingSpace = 0);
    inline Ref<ByteArray> simplify(const char *space = " \t\n\r") const { return copy()->simplifyInsitu(); }
    ByteArray *simplifyInsitu(const char *space = " \t\n\r");
    Ref<ByteArray> normalize(bool nameCase = true) const;
    Ref<ByteArray> stripTags() const;

    bool offsetToLinePos(int offset, int *line = 0, int *pos = 0) const;
    bool linePosToOffset(int line, int pos, int *offset = 0) const;

    void checkUtf8() const;

    static Ref<ByteArray> fromUtf16(ByteArray *utf16, int endian = localEndian());
    bool toUtf16(void *buf, int *size);
    Ref<ByteArray> toUtf16(int endian = localEndian());

    Ref<ByteArray> hex() const;

    bool isRootPath() const;
    bool isRelativePath() const;
    bool isAbsolutePath() const;

    Ref<ByteArray> absolutePathRelativeTo(String currentDir) const;
    Ref<ByteArray> absolutePath() const;
    Ref<ByteArray> fileName() const;
    Ref<ByteArray> baseName() const;
    Ref<ByteArray> suffix() const;
    Ref<ByteArray> reducePath() const;
    Ref<ByteArray> expandPath(String relativePath) const;
    Ref<ByteArray> canonicalPath() const;

    bool equalsCaseInsensitive(ByteArray *b) const;
    bool equalsCaseInsensitive(const char *b) const;

private:
    friend class Singleton<ByteArray>;
    friend class File;
    friend class ThreadFactory;
    friend class ByteRange;

    ByteArray(const char *data = 0, int size = -1, int flags = Terminated);
    ByteArray(const ByteArray &b);

    void destroy();

    int size_;
    union {
        char *data_;
        char *chars_;
        uint8_t *bytes_;
        uint32_t *words_;
    };

    enum Flags {
        Unterminated = 0,
        Terminated   = 1,
        Readonly     = 2,
        Wrapped      = 4,
        Mapped       = 8,
        Stack        = 16
    };
    int flags_;

    #ifndef NDEBUG
    int rangeCount_;
    #endif
};

template<class T>
int ByteArray::scanInt(T *x, int base, int i0, int i1) const
{
    int i = i0;
    if (i1 < 0) i1 = size_;
    if (i > i1) i = i1;
    *x = T();
    bool minus = false;
    if (T(-1) < T() && i < i1) {
        minus = (at(i) == '-');
        i += (at(i) == '-' || at(i) == '+');
    }
    while (i < i1) {
        char ch = at(i);
        int z = -1;
        if ('0' <= ch && ch <= '9') z = ch - '0';
        else if ('a' <= ch && ch <= 'z') z = ch - 'a';
        else if ('A' <= ch && ch <= 'Z') z = ch - 'A';
        if (z < 0 || base <= z) break;
        *x *= base;
        *x += z;
        ++i;
    }
    if (minus) *x = -*x;
    return i;
}

inline bool operator==(const ByteArray &a, const ByteArray &b) { return container::compare(a, b) == 0; }
inline bool operator!=(const ByteArray &a, const ByteArray &b) { return container::compare(a, b) != 0; }
inline bool operator< (const ByteArray &a, const ByteArray &b) { return container::compare(a, b) <  0; }
inline bool operator> (const ByteArray &a, const ByteArray &b) { return container::compare(a, b) >  0; }
inline bool operator<=(const ByteArray &a, const ByteArray &b) { return container::compare(a, b) <= 0; }
inline bool operator>=(const ByteArray &a, const ByteArray &b) { return container::compare(a, b) >= 0; }

class ByteRange: public ByteArray
{
public:
    ByteRange(ByteArray *array, int i0, int i1)
        : array_(array)
    {
        if (i0 < 0) i0 = 0;
        else if (i0 > array->size_) i0 = array->size_;
        if (i1 < i0) i1 = i0;
        else if (i1 > array->size_) i1 = array->size_;
        size_ = i1 - i0;
        data_ = array->data_ + i0;
        flags_ = ByteArray::Wrapped;
        #ifndef NDEBUG
        rangeCount_ = array->rangeCount_;
        #endif
    }

    inline operator ByteArray *() { return this; }
    inline ByteArray &operator*() { return *this; }
    inline ByteArray *operator->() { return this; }
    operator String() const;

private:
    ByteRange(const ByteRange&);
    ByteRange &operator=(const ByteRange &b);

    ByteArray *array_;
};

} // namespace flux

#endif // FLUX_BYTEARRAY_H