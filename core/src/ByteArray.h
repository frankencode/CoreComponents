/*
 * Copyright (C) 2007-2015 Frank Mertens.
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

template<class SubClass> class Singleton;

/** \brief Binary memory vector
  * \see String, File::map
  */
class ByteArray: public Object
{
public:
    typedef char Item;

    static Ref<ByteArray> create(int size = 0);
    static Ref<ByteArray> create(int size, char zero);
    static Ref<ByteArray> allocate(int size);
    static Ref<ByteArray> copy(const char *data, int size = -1);

    static Ref<ByteArray> join(const StringList *parts, const char *sep = "");
    static Ref<ByteArray> join(const StringList *parts, char sep);
    static Ref<ByteArray> join(const StringList *parts, String sep);

    ~ByteArray();

    Ref<ByteArray> clear(char zero = '\0');
    ByteArray *truncate(int newSize);
    ByteArray *resize(int newSize);

    ByteArray &operator=(const ByteArray &b);
    ByteArray &operator^=(const ByteArray &b);

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

    template<class T>
    inline bool hasItem(int j) const {
        return has(j * sizeof(T));
    }

    template<class T>
    inline T &item(int j = 0) const {
        FLUX_ASSERT(has(j * sizeof(T)) && has((j + 1) * sizeof(T) -1));
        return reinterpret_cast<T *>(data_)[j];
    }

    inline char *chars() const {
        // FLUX_ASSERT2(flags_ & Terminated, "ByteArray is not terminated by zero and therefore cannot safely be converted to a C string");
        return chars_;
    }
    inline uint8_t *bytes() const { return bytes_; }
    inline uint32_t *words() const { return words_; }
    inline operator char*() const { return chars(); }

    inline Ref<ByteArray> copy() const { return new ByteArray(*this); }
    inline Ref<ByteArray> select(int i0, int i1) { return new ByteArray(this, i0, i1); }

    template<class Range>
    inline Ref<ByteArray> copy(Range *range) const {
        if (!range) return new ByteArray();
        return copy(range->i0(), range->i1());
    }

    Ref<ByteArray> copy(int i0, int i1) const;
    Ref<ByteArray> paste(int i0, int i1, String text) const;

    inline Ref<ByteArray> head(int n) const { return copy(0, n); }
    inline Ref<ByteArray> tail(int n) const { return copy(size_ - n, size_); }

    inline bool startsWith(char ch) const {
        if (size_ == 0) return false;
        return data_[0] == ch;
    }

    inline bool endsWith(char ch) const {
        if (size_ == 0) return false;
        return data_[size_ - 1] == ch;
    }

    inline int find(char ch, int i = 0) const {
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
        for (int i = 0; i < size_; ++i)
            n += (data_[i] == ch);
        return n;
    }
    int countCharsIn(const char *set);

    inline ByteArray *replaceInsitu(char oldItem, char newItem) {
        for (int i = 0; i < size_; ++i) {
            if (data_[i] == oldItem)
                data_[i] = newItem;
        }
        return this;
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
    Ref<ByteArray> fileName() const;
    Ref<ByteArray> baseName() const;
    Ref<ByteArray> fileSuffix() const;
    Ref<ByteArray> reducePath() const;
    Ref<ByteArray> expandPath(String relativePath) const;
    Ref<ByteArray> canonicalPath() const;

    bool equalsCaseInsensitive(ByteArray *b) const;
    bool equalsCaseInsensitive(const char *b) const;

protected:
    typedef void (*Destroy)(ByteArray *array);

    ByteArray();
    ByteArray(const char *data, int size = -1, Destroy destroy = 0);
    ByteArray(ByteArray *parent, int i0, int i1);
    ByteArray(const ByteArray &b);

private:
    static void doNothing(ByteArray *);
    void destroy();

    int size_;
    union {
        char *data_;
        char *chars_;
        uint8_t *bytes_;
        uint32_t *words_;
    };

    Destroy destroy_;
    Ref<ByteArray> parent_;
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

} // namespace flux

#endif // FLUX_BYTEARRAY_H
