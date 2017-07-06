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

/** A string is a reference to a ByteArray
  * \ingroup unicode
  */
typedef Ref<ByteArray> String;

typedef List<String> StringList;

/** \class Ref<ByteArray> ByteArray.h cc/ByteArray
  * \brief Specialization of Ref<T>
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
    // explicit Ref(char ch);
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
    inline const Ref &operator=(const int &null) { CC_ASSERT(null == 0); (void)null; set(0); return *this; }

    const Ref &operator=(const char *b);

    Ref &operator+=(char b);
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
  * \brief A chunk of memory
  */
class ByteArray: public Object
{
public:
    /// Item type for iteration
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
    void fill(char zero);

    /** Shrink in size
      * \param newSize new size
      */
    void truncate(int newSize);

    /** Write contents (move bytes)
      * \param b source to copy data from
      */
    void write(const ByteArray *b) {
        int n = (size_ < b->size_) ? size_ : b->size_;
        memcpy(data_, b->data_, n);
    }

    /** Xor over another byte array
      * \param b source to xor over
      */
    void writeXor(const ByteArray *b);

    /// Size in number of bytes
    inline int count() const { return size_; }

    /** Check if index is within range
      * \param i byte index
      * \return true if i is a valid index
      */
    inline bool has(int i) const {
        return (0 <= i) && (i < size_);
    }

    /** Access character at index i
      * \param i byte index
      * \return low-level reference
      */
    inline char &at(int i) {
        CC_ASSERT(has(i));
        return data_[i];
    }

    /** Access character at index i (readonly)
      * \param i byte index
      * \return low-level reference
      */
    inline const char &at(int i) const {
        CC_ASSERT(has(i));
        return data_[i];
    }

    /** Access byte at index i
      * \param i byte index
      * \return low-level reference
      */
    inline uint8_t &byteAt(int i) {
        CC_ASSERT(has(i));
        return bytes_[i];
    }

    /** Access byte at index i (readonly)
      * \param i byte index
      * \return low-level reference
      */
    inline const uint8_t &byteAt(int i) const {
        CC_ASSERT(has(i));
        return bytes_[i];
    }

    /** Access 4-byte word at index j
      * \param j word index
      * \return low-level reference
      */
    inline uint32_t &wordAt(int j) {
        CC_ASSERT(has(j * 4) && has(j * 4 + 3));
        return words_[j];
    }

    /** Access 4-byte word at index j (readonly)
      * \param j word index
      * \return low-level reference
      */
    inline const uint32_t &wordAt(int j) const {
        CC_ASSERT(has(j * 4) && has(j * 4 + 3));
        return words_[j];
    }

    /** Check if index is within range
      * \param j item index
      * \return true if j is a valid index
      */
    template<class T>
    inline bool hasItem(int j) const {
        return has(j * sizeof(T));
    }

    /** Access item at index j
      * \param j item index
      * \return low-level reference
      */
    template<class T>
    inline T &item(int j = 0) {
        CC_ASSERT(has(j * sizeof(T)) && has((j + 1) * sizeof(T) -1));
        return reinterpret_cast<T *>(data_)[j];
    }

    /** Access item at index j (readonly)
      * \param j item index
      * \return low-level reference
      */
    template<class T>
    inline const T &item(int j = 0) const {
        CC_ASSERT(has(j * sizeof(T)) && has((j + 1) * sizeof(T) -1));
        return reinterpret_cast<T *>(data_)[j];
    }

    /// Return the internal C string representation (readonly)
    inline const char *chars() const { return chars_; }

    /// Return the internal C string representation
    inline char *chars() { return chars_; }

    /// Return byte pointer to the internal memory buffer (readonly)
    inline const uint8_t *bytes() const { return bytes_; }

    /// Return byte pointer to the internal memory buffer
    inline uint8_t *bytes() { return bytes_; }

    /// Return word pointer to the internal memory buffer (readonly)
    inline const uint32_t *words() const { return words_; }

    /// Return word pointer to the internal memory buffer
    inline uint32_t *words() { return words_; }

    /// Cast to low-level C string (readonly)
    inline operator const char*() const { return chars(); }

    /// Cast to low-level C string
    inline operator char*() { return chars(); }

    /// Create a deep copy of this string
    inline String copy() const { return new ByteArray(*this); }

    /** Check if a range is valid
      * \param i0 begin of range (index of first selected byte)
      * \param i1 end of range (index behind the last selected byte)
      * \return true if range is valid
      */
    inline bool canSelect(int i0, int i1) const { return i0 <= i1 && 0 <= i0 && i1 <= size_; }

    /** Create a selection (reference to a byte range)
      * \param i0 begin of range (index of first selected byte)
      * \param i1 end of range (index behind the last selected byte)
      * \return the selection
      */
    inline String select(int i0, int i1) const { return new ByteArray(this, i0, i1); }


    /** Return the underlying unselected string
      * \return original string
      */
    inline String unselect() const { return parent_; }

    /** Copy a range
      * \param range the range to select (range must contain i0() and i1() getters)
      * \return copied range as a string
      */
    template<class Range>
    inline String copyRange(const Range *range) const {
        if (!range) return new ByteArray();
        return copy(range->i0(), range->i1());
    }

    /** Copy a range
      * \param i0 begin of range (index of first selected character)
      * \param i1 end of range (index behind the last selected character)
      * \return copied range as a string
      */
    String copy(int i0, int i1) const;

    /** Paste into a range
      * \param i0 begin of range (index of first selected byte)
      * \param i1 end of range (index behind the last selected byte)
      * \param text text that will replace the giving range
      * \return new string with range [i0; i1[ replaced by text
      */
    String paste(int i0, int i1, const String &text) const;

    /** Copy the head of this string
      * \param n size of the head
      * \return copied string
      */
    inline String head(int n) const { return copy(0, n); }

    /** Copy the tail of this string
      * \param n size of the tail
      * \return copied string
      */
    inline String tail(int n) const { return copy(size_ - n, size_); }

    /** Check if this string starts with a certain character
      * \param ch character value to test for
      * \return true if this string starts with character ch
      */
    inline bool beginsWith(char ch) const {
        if (size_ == 0) return false;
        return data_[0] == ch;
    }

    /** Check if this string ends with a certain character
      * \param ch character value to test for
      * \return true if this string ends with character ch
      */
    inline bool endsWith(char ch) const {
        if (size_ == 0) return false;
        return data_[size_ - 1] == ch;
    }

    /** Check if this string begins with a certain prefix
      * \param s prefix to test for
      * \return true if this string begins with prefix s
      */
    bool beginsWith(const String &s) const;

    /** Check if this string begins with a certain prefix
      * \param s prefix to test for
      * \param n size of the prefix
      * \return true if this string begins with prefix s
      */
    bool beginsWith(const char *s, int n = -1) const;

    /** Check if this string ends with a certain suffix
      * \param s suffix to test for
      * \return true if this string ends with suffix s
      */
    bool endsWith(const String &s) const;

    /** Check if this string ends with a certain suffix
      * \param s suffix to test for
      * \param n size of the suffix
      * \return true if this string ends with suffix s
      */
    bool endsWith(const char *s, int n = -1) const;

    /** Find first occurence of a character value
      * \param ch character value to search for
      * \param i start search from this index
      * \return index of first occurence of character ch or count() if not found
      */
    inline int find(char ch, int i = 0) const {
        if (i < 0) i = 0;
        while (i < size_) {
            if (data_[i] == ch) break;
            ++i;
        }
        return i;
    }

    /** Check if this string contains a certain character value
      * \param ch character value to search for
      * \return true if this string contains character ch
      */
    inline bool contains(char ch) const { return find(ch) < size_; }

    /** Count the number of occurrences of a certain character value
      * \param ch character value to search for
      * \return number of occurrences of character value ch
      */
    inline int count(char ch) const {
        int n = 0;
        for (int i = 0; i < size_; ++i)
            n += (data_[i] == ch);
        return n;
    }

    /** Count number of occurrences of characters from a set of characters
      * \param set set of characters
      * \return number of occurrences of characters from the set
      */
    int countCharsIn(const char *set);

    /** Find a substring
      * \param s substring to search for
      * \param i start search from this index
      * \return index of first occurence of s or count() if not found
      */
    int find(const char *s, int i = 0) const;

    /// \copydoc find(const char *, int) const
    int find(const String &s, int i = 0) const;

    /** Check if this string contains a certain substring
      * \param s substring to search for
      * \return true if this string contains s
      */
    inline bool contains(const char *s) const
    {
        if (!s[0]) return true;
        return find(s) != size_;
    }

    /// \copydoc contains(const char *) const
    bool contains(const String &s) const;

    /** Split into pieces
      * \param sep split marker to search for
      * \return list of the different pieces without split markers
      */
    Ref<StringList> split(char sep) const;

    /// \copydoc split(char) const
    Ref<StringList> split(const char *sep) const;

    /** Break up into equal sized chunks
      * \param chunkSize size of the individual chunks
      * \return list of breakup chunks
      */
    Ref<StringList> breakUp(int chunkSize) const;

    /** Replace individual character values
      * \param oldChar old character value to search for
      * \param newChar replacement value to insert
      */
    inline void replaceInsitu(char oldChar, char newChar) {
        for (int i = 0; i < size_; ++i) {
            if (data_[i] == oldChar)
                data_[i] = newChar;
        }
    }

    /** Replace any occurrence of a certain substring by a replacement string
      * \param oldChar substring to search for
      * \param newChar replacement string
      */
    void replaceInsitu(const char *oldChar, const char *newChar);

    /** Replace individual character values
      * \param oldChar old character value to search for
      * \param newChar replacement value to insert
      * \return resulting string
      */
    String replaceEach(char oldChar, char newChar) const;

    /** Replace any occurrence of a certain substring by a replacement string
      * \param s substring to search for
      * \param r replacement string
      * \return resulting string
      */
    String replace(const char *s, const char *r) const;

    /// \copydoc replace(const char *, const char *) const
    String replace(const char *s, const String &r) const;

    /// \copydoc replace(const char *, const char *) const
    String replace(const String &s, const String &r) const;

    /** Scan a substring up to a lexical termination
      * \param x return the substring
      * \param termination lexical termination set
      * \param i0 begin of range to scan
      * \param i1 end of range to scan
      * \return scan position behind the scanned string
      */
    int scanString(String *x, const char *termination = " \t\n", int i0 = 0, int i1 = -1) const;

    /** Scan a number literal
      * \param value returns the value of the scanned number
      * \param base number base to use
      * \param i0 begin of range to scan
      * \param i1 end of range to scan
      * \return scan position behind the scanned string
      */
    template<class T>
    int scanNumber(T *value, int base = 10, int i0 = 0, int i1 = -1) const;

    /** Convert to a numerical value
      * \param ok return true if conversion was successful
      * \return numerical value
      */
    template<class T>
    inline T toNumber(bool *ok = 0) const
    {
        bool h = false;
        if (!ok) ok = &h;
        T value = T();
        *ok = (scanNumber(&value) == size_);
        return value;
    }

    /** Convert all upper-case ASCII characters to lower-case ASCII characters
      * \return conversion result
      */
    inline String toLower() const { String h = copy(); h->downcaseInsitu(); return h; }

    /** Convert all lower-case ASCII characters to upper-case ASCII characters
      * \return conversion result
      */
    inline String toUpper() const { String h = copy(); h->upcaseInsitu(); return h; }

    /// \copydoc toLower()
    inline String downcase() const { return toLower(); }

    /// \copydoc toUpper()
    inline String upcase() const { return toUpper(); }

    /// \copybrief toLower() const
    void downcaseInsitu();

    /// \copybrief toUpper() const
    void upcaseInsitu();

    /** Replace all non-printable and non-ASCII characters by escape sequences
      * \return escaped string
      */
    String escape() const;

    /** Replace all escape sequences by their character value
      * \return unescaped string
      */
    inline String unescape() const { String h = copy(); h->unescapeInsitu(); return h; }

    /// \copybrief unescape() const
    void unescapeInsitu();

    /** Remove leading and trailing whitespace
      * \param leadingSpace set of characters to detect as leading whitespace
      * \param trailingSpace set of characters to detect as trailing whitespace
      * \return trimmed string
      */
    inline String trim(const char *leadingSpace = " \t\n\r", const char *trailingSpace = 0) const { String h = copy(); h->trimInsitu(leadingSpace, trailingSpace); return h; }

    /** Remove leading whitespace
      * \param space set of characters to detect as whitespace
      * \return trimmed string
      */
    inline String trimLeading(const char *space = " \t\n\r") const { String h = copy(); h->trimInsitu(space, ""); return h; }

    /** Remove trailing whitespace
      * \param space set of characters to detect as whitespace
      * \return trimmed string
      */
    inline String trimTrailing(const char *space = " \t\n\r") const { String h = copy(); h->trimInsitu("", space); return h; }

    /** \copybrief trim(const char *, const char *) const
      */
    void trimInsitu(const char *leadingSpace = " \t\n\r", const char *trailingSpace = 0);

    /** Replace multi character whitespaces by single character whitespaces
      * \return simplified string
      */
    inline String simplify(const char *space = " \t\n\r") const { String h = copy(); h->simplifyInsitu(space); return h; }

    /// \copybrief simplify(const char *) const
    void simplifyInsitu(const char *space = " \t\n\r");

    /** Common user input normalization
      * \param nameCase make sure the first character is upper case
      * \return normalized string
      */
    String normalize(bool nameCase = true) const;

    /** Remove all XML/HTML tags and entities
      * \return santized string
      */
    String xmlSanitize() const;

    /** \brief %Map a byte offset to editor coordinates.
      * \param offset byte offset
      * \param line n-th line starting with 1
      * \param pos position on line starting with 0 (in bytes)
      * \return true if offset is within valid range
      */
    bool offsetToLinePos(int offset, int *line = 0, int *pos = 0) const;

   /** %Map editor coordinates to a byte offset
     * \param line n-th line starting with 1
     * \param pos position on line starting with 1 (in bytes)
     * \param offset byte offset
     * \return true if successful
     */
    bool linePosToOffset(int line, int pos, int *offset = 0) const;

    /** Check if this string is UTF-8 encoded
      * \exception EncodingError this string is not UTF-8 encoded
      */
    void checkUtf8() const;

    /** Create an UTF-8 encoded string from an UTF-16 encoded string
      * \param utf16 UTF-16 encoded string
      * \param endian endianess of the UTF-16 encoded string
      * \return UTF-8 encoded string
      */
    static String fromUtf16(const String &utf16, Endian endian = localEndian());

   /** Convert to a UTF-16 encoded string (local endian).
     * \param buf destination buffer
     * \param size pass capacity of destination buffer
     * \return true if the given buffer was suitable to hold the encoded string
     *
     * The number of bytes required to fully represent the string in UTF-16 is
     * returned with the 'size' argument. Passing a zero for 'size' allows to
     * determine the required buffer size. No zero termination is written or
     * or accounted for.
     */
    bool toUtf16(void *buf, int *size);

    /** \copybrief toUtf16(void *, int *)
      * \param endian endianess of the UTF-16 encoded string
      * \return UTF-16 encoded string
      */
    String toUtf16(Endian endian = localEndian()) const;

    /** Convert to a series of hexadecimal characters
      * \see hexDump
      */
    String toHex() const;

    /** Convert to a hexadecimal dump
      * \see HexDump
      */
    String hexDump() const;

    /// Add a prefix to each line of the text
    String indent(String prefix) const;

    /// Check if this string equals "/"
    bool isRootPath() const;

    /// Check if this string represents a relative path
    bool isRelativePath() const;

    /// Check if this string represents an absolute path
    bool isAbsolutePath() const;

    /** Convert to an absolute
      * \param currentDir absolute path to the current directory
      * \return absolute path
      */
    String absolutePathRelativeTo(const String &currentDir) const;

    /// Return the filename component
    String fileName() const;

    /// Return the filename component without file type suffix
    String baseName() const;

    /// Return the file type suffix
    String fileSuffix() const;

    /// Remove the last component from this path
    String reducePath() const;

    /** Add components to this path
      * \param relativePath relative path to add
      * \return resulting path
      */
    String extendPath(const String &relativePath) const;

    String expandPath(const String &relativePath) const { return extendPath(relativePath); } /// FIXME: obsolete

    /// Remove redundant or ambigous path components
    String canonicalPath() const;

    /// Compare for identity with b
    bool equals(const String &b) const { return String(this) == b; }

    /** Compare with another string (case-insensitive)
      * \param b other string
      * \return true if equals
      */
    bool equalsCaseInsensitive(const String &b) const;

    /// \copydoc equalsCaseInsensitive(const String &) const
    bool equalsCaseInsensitive(const char *b) const;

    /** STL-style iterator declarations
      * @{
      */
    typedef Iterator<ByteArray> iterator;
    iterator begin() { return iterator(this, 0); }
    iterator end() { return iterator(this, count()); }
    /** @} */

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
int ByteArray::scanNumber(T *value, int base, int i0, int i1) const
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
            *value = cc::nan();
            return i + 3;
        }
        else if (at(i) == 'i' && at(i + 1) == 'n' && at(i + 2) == 'f') {
            *value = sign * cc::inf();
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
                i = scanNumber(&ep, base, i + 1, i1);
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

/*inline Ref<ByteArray>::Ref(char ch):
    a_(0)
{
    set(ByteArray::create(1));
    a_->at(0) = ch;
}*/

inline Ref<ByteArray>::Ref(int size, char zero):
    a_(0)
{
    Ref<ByteArray> b = ByteArray::create(size);
    b->fill(zero);
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

inline Ref<ByteArray> &Ref<ByteArray>::operator+=(char b) { String s(1); s->at(0) = b; set(ByteArray::cat(*this, s)); return *this; }
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
