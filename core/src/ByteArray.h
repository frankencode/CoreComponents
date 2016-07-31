/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/containers>
#include <cc/strings>
#include <cc/List>
#ifdef QT_CORE_LIB
#include <QString>
#include <QUrl>
#endif

namespace cc {

class ByteArray;
class Variant;
class Format;

/** A String is simply an alias of Ref<ByteArray>
  */
typedef Ref<ByteArray> String;

typedef List<String> StringList;

/** \brief Specialization of Ref<T>
  */
template<>
class Ref<ByteArray>
{
public:
    Ref(): a_(0) {}
    Ref(Ref &&b): a_(b.a_) { b.a_ = 0; }
    Ref(const Ref &b);

    Ref(const ByteArray *b);
    Ref(ByteArray *b): a_(0) { set(b); }

    Ref(const char *other, int size = -1);
    explicit Ref(int size, char zero = '\0');

    Ref(const Ref<StringList> &parts);
    Ref(const Variant &b);
    Ref(const Format &b);

    ~Ref() { set(0); }

    static Ref copy(const Ref &b);

    inline const Ref &operator=(Ref &&b) { set(0); a_ = b.a_; b.a_ = 0; return *this; }
    inline const Ref &operator=(const Ref &b);
    inline const Ref &operator=(ByteArray *b) { set(b); return *this; }

    inline const Ref &operator=(const Ref<StringList> &b) { set(String(b)); return *this; }
    inline const Ref &operator=(const Variant &b) { set(String(b)); return *this; }
    inline const Ref &operator=(const Format &b) { set(String(b)); return *this; }
    inline const Ref &operator=(const int &null) { CC_ASSERT(null == 0); set(0); return *this; }

    const Ref &operator=(const char *b);

    Ref &operator+=(const char *b);
    Ref &operator+=(const String &b);
    Ref &operator+=(const Ref<StringList> &b);

    operator char *() const;
    inline operator ByteArray *() const { return get(); }
    inline operator bool() const { return a_; }

    inline ByteArray &operator*() const { return *get(); }
    inline ByteArray *operator->() { return get(); }
    inline const ByteArray *operator->() const { return get(); }

    ByteArray *get() const; // FIXME: make static
    void set(ByteArray *b); // FIXME: make static

    static Ref<ByteArray> join(const StringList *parts, String sep = "");

#ifdef QT_CORE_LIB
    inline operator QString() const;
    inline operator QUrl() const;
#endif

private:
    static ByteArray *getDefault();
    ByteArray *a_;
};

/** \class ByteArray ByteArray.h cc/ByteArray
  * \brief A chunk of memory as a series of bytes
  */
class ByteArray: public Object
{
public:
    typedef char Item;

    /** Create a new byte array of exactly N bytes, without zero-termination
      * \param size number of bytes (N)
      * \return new object instance
      * \see isZeroTerminated()
      */
    static String allocate(int size);

    /** Create a new byte array of N bytes plus one byte for zero termination
      * \param size number of bytes (N)
      * \return new object instance
      * \see isZeroTerminated()
      */
    static String create(int size = 0);

    /** Create a copy of a C string
      * \param data pointer to C string
      * \param size number of bytes (if known, else pass -1)
      * \return new object instance
      * \see isZeroTerminated()
      */
    static String copy(const char *data, int size = -1);

    /** Create a copy of another byte array
      * \param b the original byte array
      * \return new object instance
      */
    inline static String copy(const ByteArray *b) { return b ? b->copy() : String(); }

    /** Join a list of strings
      * \param parts the individual parts to join together
      * \param sep the separator to put between individual parts
      * \param sepSize the size of the separator (if kwown, else pass -1)
      * \return new object instance
      */
    static String join(const StringList *parts, const char *sep = "", int sepSize = -1);

    /** Join a list of strings
      * \param parts the individual parts to join together
      * \param sep the separator to put between individual parts
      * \return new object instance
      */
    static String join(const StringList *parts, char sep);

    /** Join a list of strings
      * \param parts the individual parts to join together
      * \param sep the separator to put between individual parts
      * \return new object instance
      */
    static String join(const StringList *parts, const String &sep);

    /** Concatenate two strings
      * \param a the left string
      * \param b the right string
      * \return new object instance
      */
    static String cat(const String &a, const String &b);

    /// Return the default empty byte array
    static ByteArray *empty();

    /// Returns true if this byte array is zero terminated
    virtual bool isZeroTerminated() const { return true; }

    /** Clear all bytes
      * \param zero blank character to use
      */
    void clear(char zero = '\0');

    /** Shrink in size
      * \param newSize new size
      */
    void truncate(int newSize);

    /** Write contents (move bytes)
      * \param b source to copy
      */
    ByteArray &operator=(const ByteArray &b); // FIXME: rename write

    /** Xor over another byte array
      * \param b source to xor over
      * \return low-level reference
      */
    ByteArray &operator^=(const ByteArray &b); // FIXME: rename xorOver

    /// size in number of bytes
    inline int count() const { return size_; }

    inline bool has(int i) const {
        return (0 <= i) && (i < size_);
    }

    inline char &at(int i) {
        CC_ASSERT(has(i));
        return data_[i];
    }
    inline const char &at(int i) const {
        CC_ASSERT(has(i));
        return data_[i];
    }

    inline uint8_t &byteAt(int i) {
        CC_ASSERT(has(i));
        return bytes_[i];
    }
    inline const uint8_t &byteAt(int i) const {
        CC_ASSERT(has(i));
        return bytes_[i];
    }

    inline uint32_t &wordAt(int j) {
        CC_ASSERT(has(j * 4) && has(j * 4 + 3));
        return words_[j];
    }
    inline const uint32_t &wordAt(int j) const {
        CC_ASSERT(has(j * 4) && has(j * 4 + 3));
        return words_[j];
    }

    template<class T>
    inline bool hasItem(int j) const {
        return has(j * sizeof(T));
    }

    template<class T>
    inline T &item(int j = 0) const {
        CC_ASSERT(has(j * sizeof(T)) && has((j + 1) * sizeof(T) -1));
        return reinterpret_cast<T *>(data_)[j];
    }

    inline char *chars() {
        CC_ASSERT2(isZeroTerminated(), "ByteArray is not terminated by zero and therefore cannot safely be converted to a C string");
        return chars_;
    }
    inline const char *chars() const {
        CC_ASSERT2(isZeroTerminated(), "ByteArray is not terminated by zero and therefore cannot safely be converted to a C string");
        return chars_;
    }

    inline const uint8_t *bytes() const { return bytes_; }
    inline uint8_t *bytes() { return bytes_; }

    inline const uint32_t *words() const { return words_; }
    inline uint32_t *words() { return words_; }

    inline operator const char*() const { return chars(); }
    inline operator char*() { return chars(); }

    inline String copy() const { return new ByteArray(*this); }

    inline bool canSelect(int i0, int i1) const { return i0 <= i1 && 0 <= i0 && i1 <= size_; }
    inline String select(int i0, int i1) const { return new ByteArray(this, i0, i1); }

    template<class Range>
    inline String copyRange(const Range *range) const {
        if (!range) return new ByteArray();
        return copy(range->i0(), range->i1());
    }

    String copy(int i0, int i1) const;
    String paste(int i0, int i1, const String &text) const;

    inline String head(int n) const { return copy(0, n); }
    inline String tail(int n) const { return copy(size_ - n, size_); }

    inline bool beginsWith(char ch) const {
        if (size_ == 0) return false;
        return data_[0] == ch;
    }

    inline bool endsWith(char ch) const {
        if (size_ == 0) return false;
        return data_[size_ - 1] == ch;
    }

    bool beginsWith(const String &s) const;
    bool beginsWith(const char *s, int n = -1) const;

    bool endsWith(const String &s) const;
    bool endsWith(const char *s, int n = -1) const;

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

    int find(const char *pattern, int i = 0) const;
    int find(const String &pattern, int i = 0) const;

    inline bool contains(const char *pattern) const { return find(pattern) != size_; }
    bool contains(const String &pattern) const;

    Ref<StringList> split(char sep) const;
    Ref<StringList> split(const char *sep) const;
    Ref<StringList> breakUp(int chunkSize) const;

    inline String replaceInsitu(char oldItem, char newItem) {
        for (int i = 0; i < size_; ++i) {
            if (data_[i] == oldItem)
                data_[i] = newItem;
        }
        return this;
    }

    String replaceInsitu(const char *pattern, const char *replacement);
    String replace(const char *pattern, const char *replacement) const;
    String replace(const char *pattern, const String &replacement) const;
    String replace(const String &pattern, const String &replacement) const;

    int scanString(String *x, const char *termination = " \t\n", int i0 = 0, int i1 = -1) const;

    template<class T>
    int scan(T *value, int base = 10, int i0 = 0, int i1 = -1) const;

    template<class T>
    inline T toNumber(bool *ok = 0) const
    {
        bool h = false;
        if (!ok) ok = &h;
        T value = T();
        *ok = (scan(&value) == size_);
        return value;
    }

    inline String toLower() const { return copy()->downcaseInsitu(); }
    inline String toUpper() const { return copy()->upcaseInsitu(); }
    inline String downcase() const { return copy()->downcaseInsitu(); } // FIXME: obsolete
    inline String upcase() const { return copy()->upcaseInsitu(); } // FIXME: obsolete
    String downcaseInsitu();
    String upcaseInsitu();

    String escape() const;
    inline String unescape() const { return copy()->unescapeInsitu(); }
    String unescapeInsitu();

    inline String trim(const char *leadingSpace = " \t\n\r", const char *trailingSpace = 0) const { return copy()->trimInsitu(leadingSpace, trailingSpace); }
    inline String trimLeading(const char *space = " \t\n\r") const { return copy()->trimInsitu(space, ""); }
    inline String trimTrailing(const char *space = " \t\n\r") const { return copy()->trimInsitu("", space); }
    String trimInsitu(const char *leadingSpace = " \t\n\r", const char *trailingSpace = 0);
    inline String simplify(const char *space = " \t\n\r") const { return copy()->simplifyInsitu(); }
    String simplifyInsitu(const char *space = " \t\n\r");
    String normalize(bool nameCase = true) const;
    String stripTags() const;

    bool offsetToLinePos(int offset, int *line = 0, int *pos = 0) const;
    bool linePosToOffset(int line, int pos, int *offset = 0) const;

    void checkUtf8() const;

    static String fromUtf16(const String &utf16, Endian endian = localEndian());
    bool toUtf16(void *buf, int *size);
    String toUtf16(Endian endian = localEndian());

    /// Convert to a series of hexadecimal characters
    String toHex() const;

    bool isRootPath() const;
    bool isRelativePath() const;
    bool isAbsolutePath() const;

    String absolutePathRelativeTo(const String &currentDir) const;
    String fileName() const;
    String baseName() const;
    String fileSuffix() const;
    String reducePath() const;
    String expandPath(const String &relativePath) const;
    String canonicalPath() const;

    bool equalsCaseInsensitive(const String &b) const;
    bool equalsCaseInsensitive(const char *b) const;

protected:
    friend class Ref<ByteArray>;

    friend bool operator==(const String &a, const String &b);
    friend bool operator!=(const String &a, const String &b);
    friend bool operator< (const String &a, const String &b);
    friend bool operator<=(const String &a, const String &b);

    typedef void (*Destroy)(ByteArray *array);

    ByteArray();
    ByteArray(const char *data, int size = -1, Destroy destroy = 0);
    ByteArray(const ByteArray *parent, int i0, int i1);
    ByteArray(const ByteArray &b);

    /// Lowlevel destructor
    ~ByteArray();

private:
    static void doNothing(ByteArray *);
    void destroy();

    static double pow(double x, double y);

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
int ByteArray::scan(T *value, int base, int i0, int i1) const
{
    int i = i0;
    if (i1 < 0) i1 = size_;
    if (i > i1) i = i1;
    int sign = 1;
    if (T(-1) < T() && i < i1) {
        if (at(i) == '-') sign = -1;
        i += (at(i) == '-' || at(i) == '+');
    }
    bool isFloating = (T(1)/T(3) > 0);
    if (isFloating && i + 2 < i1) {
        if (at(i) == 'n' && at(i + 1) == 'a' && at(i + 2) == 'n') {
            *value = cc::nan;
            return i + 3;
        }
        else if (at(i) == 'i' && at(i + 1) == 'n' && at(i + 2) == 'f') {
            *value = sign * cc::inf;
            return i + 3;
        }
    }
    if (i < i1) {
        if (at(i) == '0') {
            if (i + 1 < i1) {
                char ch = at(i + 1);
                if (ch == 'x') { base = 16; i += 2; }
                else if (ch == 'b') { base = 2; i += 2; }
                else if ('0' <= ch && ch <= '7') { base = 8; i += 1; }
            }
        }
    }
    T x = 0;
    while (i < i1) {
        char ch = at(i);
        int z = -1;
        if ('0' <= ch && ch <= '9') z = ch - '0';
        else if ('a' <= ch && ch <= 'z') z = 10 + ch - 'a';
        else if ('A' <= ch && ch <= 'Z') z = 10 + ch - 'A';
        if (z < 0 || base <= z) break;
        T y = x;
        x = x * base + sign * z;
        if (!isFloating) {
            if (sign > 0 ? x < y : y < x)
                break;
        }
        ++i;
    }
    if (isFloating && i < i1) {
        if (at(i) == '.') {
            ++i;
            for (T h = T(sign) / T(base); i < i1; ++i) {
                char ch = at(i);
                int z = -1;
                if ('0' <= ch && ch <= '9') z = ch - '0';
                else if ('a' <= ch && ch <= 'z') z = 10 + ch - 'a';
                else if ('A' <= ch && ch <= 'Z') z = 10 + ch - 'A';
                if (z < 0 || base <= z) break;
                x += h * z;
                h /= base;
            }
        }
        if (i + 1 < i1) {
            if (at(i) == 'E' || at(i) == 'e') {
                int ep = 0;
                i = scan(&ep, base, i + 1, i1);
                x *= pow(T(base), T(ep));
            }
        }
    }
    *value = x;
    return i;
}

inline bool operator==(const String &a, const String &b) { return a->size_ == b->size_ && strncmp(a->chars_, b->chars_, a->size_) == 0; }
inline bool operator!=(const String &a, const String &b) { return a->size_ != b->size_ || strncmp(a->chars_, b->chars_, a->size_) != 0; }
inline bool operator< (const String &a, const String &b) {
    int m = a->size_ < b->size_ ? a->size_ : b->size_;
    int ret = strncmp(a->chars_, b->chars_, m);
    if (ret == 0) return a->size_ < b->size_;
    return ret < 0;
}
inline bool operator> (const String &a, const String &b) { return b < a; }
inline bool operator<=(const String &a, const String &b) {
    int m = a->size_ < b->size_ ? a->size_ : b->size_;
    int ret = strncmp(a->chars_, b->chars_, m);
    if (ret == 0) return a->size_ <= b->size_;
    return ret < 0;
}
inline bool operator>=(const String &a, const String &b) { return b <= a; }

inline bool operator==(const char *a, const String &b) { return strcmp(a, b->chars()) == 0; }
inline bool operator!=(const char *a, const String &b) { return strcmp(a, b->chars()) != 0; }
inline bool operator< (const char *a, const String &b) { return strcmp(a, b->chars()) <  0; }
inline bool operator> (const char *a, const String &b) { return strcmp(a, b->chars()) >  0; }
inline bool operator<=(const char *a, const String &b) { return strcmp(a, b->chars()) <= 0; }
inline bool operator>=(const char *a, const String &b) { return strcmp(a, b->chars()) >= 0; }

inline bool operator==(char *a, const String &b) { return strcmp(a, b->chars()) == 0; }
inline bool operator!=(char *a, const String &b) { return strcmp(a, b->chars()) != 0; }
inline bool operator< (char *a, const String &b) { return strcmp(a, b->chars()) <  0; }
inline bool operator> (char *a, const String &b) { return strcmp(a, b->chars()) >  0; }
inline bool operator<=(char *a, const String &b) { return strcmp(a, b->chars()) <= 0; }
inline bool operator>=(char *a, const String &b) { return strcmp(a, b->chars()) >= 0; }

inline bool operator==(const String &a, const char *b) { return strcmp(a->chars(), b) == 0; }
inline bool operator!=(const String &a, const char *b) { return strcmp(a->chars(), b) != 0; }
inline bool operator< (const String &a, const char *b) { return strcmp(a->chars(), b) <  0; }
inline bool operator> (const String &a, const char *b) { return strcmp(a->chars(), b) >  0; }
inline bool operator<=(const String &a, const char *b) { return strcmp(a->chars(), b) <= 0; }
inline bool operator>=(const String &a, const char *b) { return strcmp(a->chars(), b) >= 0; }

inline bool operator==(const String &a, char *b) { return strcmp(a->chars(), b) == 0; }
inline bool operator!=(const String &a, char *b) { return strcmp(a->chars(), b) != 0; }
inline bool operator< (const String &a, char *b) { return strcmp(a->chars(), b) <  0; }
inline bool operator> (const String &a, char *b) { return strcmp(a->chars(), b) >  0; }
inline bool operator<=(const String &a, char *b) { return strcmp(a->chars(), b) <= 0; }
inline bool operator>=(const String &a, char *b) { return strcmp(a->chars(), b) >= 0; }

inline Ref<ByteArray>::Ref(const Ref &b):
    a_(0)
{
    set(b.a_);
}

inline Ref<ByteArray>::Ref(const ByteArray *b):
    a_(0)
{
    set(ByteArray::copy(b));
}

inline Ref<ByteArray>::Ref(const char *other, int size):
    a_(0)
{
    set(ByteArray::copy(other, size));
}

inline Ref<ByteArray>::Ref(int size, char zero):
    a_(0)
{
    Ref<ByteArray> b = ByteArray::create(size);
    b->clear(zero);
    set(b);
}

inline Ref<ByteArray>::Ref(const Ref<StringList> &parts):
    a_(0)
{
    if (parts)
        set(ByteArray::join(parts));
}

inline Ref<ByteArray> Ref<ByteArray>::copy(const Ref &b)
{
    return ByteArray::copy(b.a_);
}

inline const Ref<ByteArray> &Ref<ByteArray>::operator=(const Ref &b)
{
    set(b.a_);
    return *this;
}

inline const Ref<ByteArray> &Ref<ByteArray>::operator=(const char *b)
{
    set(ByteArray::copy(b));
    return *this;
}

inline Ref<ByteArray>::operator char *() const { return get()->chars(); }

inline Ref<StringList> operator+(const String &a, const String &b) { return StringList::create() << a << b; }
inline Ref<StringList> operator+(Ref<StringList> &a, const String &b) { return a << b; }
inline Ref<StringList> operator+(const String &a, const char *b) { return a + String(b); }
inline Ref<StringList> operator+(const char *a, const String& b) { return String(a) + b; }

inline Ref<ByteArray> &Ref<ByteArray>::operator+=(const char *b) { set(ByteArray::cat(*this, String(b))); return *this; }
inline Ref<ByteArray> &Ref<ByteArray>::operator+=(const String &b) { set(ByteArray::cat(*this, b)); return *this; }
inline Ref<ByteArray> &Ref<ByteArray>::operator+=(const Ref<StringList> &b) { set(String(*this + b)); return *this; }

inline Ref<ByteArray> Ref<ByteArray>::join(const StringList *parts, String sep) { return ByteArray::join(parts, sep); }

inline ByteArray *Ref<ByteArray>::get() const
{
    if (!a_) return ByteArray::empty();
    return a_;
}

inline void Ref<ByteArray>::set(ByteArray *b)
{
    if (a_ != b) {
        if (b) b->incRefCount();
        if (a_) a_->decRefCount();
        a_ = b;
    }
}

#ifdef QT_CORE_LIB
inline Ref<ByteArray>::operator QString() const
{
    return QString::fromUtf8(get()->chars(), get()->count());
}

inline Ref<ByteArray>::operator QUrl() const
{
    return QUrl::fromLocalFile(QString::fromUtf8(get()->chars(), get()->count()));
}
#endif

} // namespace cc
