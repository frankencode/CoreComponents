/*
 * Copyright (C) 2007-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <math.h>
#include <cc/List>
#include <cc/Unicode>
#include <cc/Utf8Source>
#include <cc/Utf8Sink>
#include <cc/Utf16Source>
#include <cc/Utf16Sink>
#include <cc/Format>
#include <cc/HexDump>
#include <cc/ByteArray>

namespace cc {

Ref<ByteArray>::Ref(const Variant &b):
    a_(0)
{
    set(Variant::toString(b));
}

Ref<ByteArray>::Ref(const Format &b):
    a_(0)
{
    set(b->join());
}

class Chunk: public ByteArray
{
public:
    bool isZeroTerminated() const { return false; }

private:
    friend class ByteArray;
    Chunk(char *data, int size):
        ByteArray(data, size)
    {}
};

String ByteArray::allocate(int size)
{
    if (size <= 0) return new ByteArray();
    return new Chunk(new char[size], size);
}

String ByteArray::create(int size)
{
    if (size <= 0) return new ByteArray();
    char *data = new char[size + 1];
    data[size] = 0;
    return new ByteArray(data, size);
}

String ByteArray::copy(const char *data, int size)
{
    if (!data) return new ByteArray();
    if (size < 0) size = strlen(data);
    if (size <= 0) return new ByteArray();
    char *newData = new char[size + 1];
    newData[size] = 0;
    memcpy(newData, data, size);
    return new ByteArray(newData, size);
}

String ByteArray::join(const StringList *parts, const char *sep, int sepSize)
{
    if (!parts) return String();

    if (parts->count() == 0)
        return String();

    if (sepSize < 0) sepSize = strlen(sep);
    int size = 0;
    for (int i = 0; i < parts->count(); ++i)
        size += parts->at(i)->count();
    size += (parts->count() - 1) * sepSize;
    String result(size);
    char *p = result->data_;
    for (int i = 0; i < parts->count(); ++i) {
        const ByteArray *part = parts->at(i);
        memcpy(p, part->data_, part->size_);
        p += part->size_;
        if (i + 1 < parts->count()) {
            memcpy(p, sep, sepSize);
            p += sepSize;
        }
    }
    CC_ASSERT(p == result->data_ + result->size_);
    return result;
}

String ByteArray::join(const StringList *parts, const String &sep)
{
    return join(parts, sep->chars(), sep->count());
}

String ByteArray::cat(const String &a, const String &b)
{
    String c(a->count() + b->count());
    memcpy(c->data_, a->data_, a->size_);
    memcpy(c->data_ + a->size_, b->data_, b->size_);
    return c;
}

const ByteArray *ByteArray::empty()
{
    static thread_local String empty_(new ByteArray());
    return empty_;
}

ByteArray::ByteArray():
    size_(0),
    data_(new char[1]),
    destroy_(0)
{
    data_[0] = 0;
}

ByteArray::ByteArray(const char *data, int size, Destroy destroy)
{
    if (data) {
        if (size < 0) size = strlen(data);
        if (size > 0) {
            size_ = size;
            data_ = const_cast<char *>(data);
            destroy_ = destroy;
        }
    }
    if (size == 0) {
        const ByteArray *b = ByteArray::empty();
        size_ = b->size_;
        data_ = b->data_;
        destroy_ = b->destroy_;
    }
}

ByteArray::ByteArray(const ByteArray *parent, int i0, int i1):
    destroy_(doNothing),
    parent_(const_cast<ByteArray *>(parent))
{
    if (i0 < 0) i0 = 0;
    else if (i0 > parent->size_) i0 = parent_->size_;
    if (i1 < i0) i1 = i0;
    else if (i1 > parent->size_) i1 = parent_->size_;
    size_ = i1 - i0;
    data_ = parent_->data_ + i0;
}

ByteArray::ByteArray(const ByteArray &b):
    size_(0),
    data_(const_cast<char *>("")),
    destroy_(doNothing)
{
    if (b.size_ > 0) {
        size_ = b.size_;
        data_ = new char[b.size_ + 1];
        destroy_ = 0;
        memcpy(data_, b.data_, b.size_);
        data_[size_] = 0;
    }
}

ByteArray::~ByteArray()
{
    destroy();
}

void ByteArray::doNothing(ByteArray *)
{}

void ByteArray::destroy()
{
    if (destroy_) destroy_(this);
    else delete[] data_;
}

String ByteArray::fill(char zero)
{
    memset(data_, zero, size_);
    return this;
}

void ByteArray::truncate(int newSize)
{
    if (newSize < size_) {
        if (newSize < 0) newSize = 0;
        if (newSize > size_) newSize = size_;
        size_ = newSize;
        data_[size_] = 0;
    }
}

void ByteArray::writeXor(const ByteArray *b)
{
    int n = (size_ < b->size_) ? size_ : b->size_;
    for (int i = 0; i < n; ++i)
        bytes_[i] ^= b->bytes_[i];
}

String ByteArray::copy(int i0, int i1) const
{
    if (i0 < 0) i0 = 0;
    if (i0 > size_) i0 = size_;
    if (i1 < 0) i1 = 0;
    if (i1 > size_) i1 = size_;
    int newSize = i1 - i0;
    if (newSize <= 0) return create();
    char *newData = new char[newSize + 1];
    memcpy(newData, data_ + i0, newSize);
    newData[newSize] = 0;
    return new ByteArray(newData, newSize);
}

String ByteArray::paste(int i0, int i1, const String &text) const
{
    Ref<StringList> parts = StringList::create();
    parts->append(copy(0, i0));
    parts->append(text);
    parts->append(copy(i1, size_));
    return parts->join();
}

int ByteArray::countCharsIn(const char *set) const
{
    int n = 0;
    for (const char *p = data_; *p; ++p) {
        for (const char *s = set; *s; ++s)
            n += (*p == *s);
    }
    return n;
}

bool ByteArray::beginsWith(const String &s) const
{
    return beginsWith(s->chars(), s->count());
}

bool ByteArray::beginsWith(const char *s, int n) const
{
    if (n < 0) n = strlen(s);
    if (n > size_) return false;

    for (int i = 0; i < n; ++i) {
        if (chars_[i] != s[i])
            return false;
    }
    return true;
}

bool ByteArray::endsWith(const String &s) const
{
    return endsWith(s->chars(), s->count());
}

bool ByteArray::endsWith(const char *s, int n) const
{
    if (n < 0) n = strlen(s);
    if (n > size_) return false;

    for (int i = size_ - n, j = 0; i < size_; ++i, ++j) {
        if (chars_[i] != s[j])
            return false;
    }
    return true;
}

int ByteArray::find(const char *s, int i) const
{
    if (!has(i)) return size_;
    if (!s[0]) return size_;
    for (int j = i, k = 0; j < size_;) {
        if (data_[j++] == s[k]) {
            ++k;
            if (!s[k])
                return j - k;
        }
        else {
            k = 0;
        }
    }
    return size_;
}

int ByteArray::find(const String &s, int i) const
{
    return find(s->chars(), i);
}

bool ByteArray::contains(const String &s) const
{
    return contains(s->chars());
}

Ref<StringList> ByteArray::split(char sep) const
{
    Ref<StringList> parts = StringList::create();
    for (int i = 0; i < size_;) {
        int j = find(sep, i);
        parts->append(copy(i, j));
        i = j + 1;
        if (i == size_) {
            parts->append("");
            break;
        }
    }
    return parts;
}

Ref<StringList> ByteArray::split(const char *sep) const
{
    Ref<StringList> parts = StringList::create();
    int i0 = 0;
    int sepLength = strlen(sep);
    while (i0 < size_) {
        int i1 = find(sep, i0);
        if (i1 == size_) break;
        parts->append(copy(i0, i1));
        i0 = i1 + sepLength;
    }
    if (i0 < size_)
        parts->append(copy(i0, size_));
    else
        parts->append(String());
    return parts;
}

Ref<StringList> ByteArray::breakUp(int chunkSize) const
{
    Ref<StringList> parts = StringList::create();
    int i0 = 0;
    while (i0 < size_) {
        int i1 = i0 + chunkSize;
        if (i1 > size_) i1 = size_;
        parts->append(copy(i0, i1));
        i0 += chunkSize;
    }
    return parts;
}

String ByteArray::replaceInsitu(const char *pattern, const char *replacement)
{
    int patternLength = strlen(pattern);
    int replacementLength = strlen(replacement);
    if (patternLength < replacementLength) {
        write(replace(pattern, replacement));
    }
    else if (patternLength > 0) {
        int i = 0, j = 0, k = 0, n = size_;
        while (i < n) {
            char ch = data_[i++];
            if (j < i) data_[j++] = ch;
            if (ch == pattern[k]) {
                ++k;
                if (k == patternLength) {
                    j -= patternLength;
                    for (k = 0; k < replacementLength; ++k)
                        data_[j++] = replacement[k];
                    k = 0;
                }
            }
            else {
                k = 0;
            }
        }
        truncate(j);
    }
    return this;
}

String ByteArray::replaceEach(char s, char r) const
{
    return mutate(copy())->replaceInsitu(s, r);
}

String ByteArray::replace(const char *s, const char *r) const
{
    return join(split(s), r);
}

String ByteArray::replace(const char *s, const String &r) const
{
    return replace(s, r->chars());
}

String ByteArray::replace(const String &s, const String &r) const
{
    return replace(s->chars(), r->chars());
}

int ByteArray::scanString(String *x, const char *termination, int i0, int i1) const
{
    if (i1 < 0 || i1 > size_) i1 = size_;
    if (i0 > i1) i0 = i1;
    int i = i0;
    for (; i < i1; ++i) {
        const char *p = termination;
        if (!at(i)) break;
        for(; *p; ++p) {
            if (at(i) == *p) break;
        }
        if (*p) break;
    }
    *x = copy(i0, i);
    return i;
}

String ByteArray::downcaseInsitu()
{
    for (int i = 0; i < size_; ++i)
        chars_[i] = cc::downcase(chars_[i]);
    return this;
}

String ByteArray::upcaseInsitu()
{
    for (int i = 0; i < size_; ++i)
        data_[i] = cc::upcase(data_[i]);
    return this;
}

String ByteArray::escape() const
{
    Ref<StringList> parts;
    int i = 0, i0 = 0;
    for (; i < size_; ++i) {
        uint8_t ch = bytes_[i];
        if (ch < 32 || 127 <= ch) {
            if (!parts) parts = StringList::create();
            if (i0 < i) parts->append(copy(i0, i));
            i0 = i + 1;
            if (ch == 0x08) parts->append("\\b");
            else if (ch == 0x09) parts->append("\\t");
            else if (ch == 0x0A) parts->append("\\n");
            else if (ch == 0x0D) parts->append("\\r");
            else if (ch == 0x0C) parts->append("\\f");
            else {
                String s = "\\xXX";
                const char *hex = "0123456789ABCDEF";
                mutate(s)->at(s->count() - 2) = hex[ch / 16];
                mutate(s)->at(s->count() - 1) = hex[ch % 16];
                parts->append(s);
            }
        }
    }
    if (!parts) return copy();

    if (i0 < i) parts->append(copy(i0, i));

    return parts->join();
}

String ByteArray::unescapeInsitu()
{
    if (!contains('\\')) return this;
    int j = 0;
    uint32_t hs = 0; // high surrogate, saved
    String ec; // buffer for encoded character
    for (int i = 0, n = size_; i < n;) {
        char ch = data_[i++];
        if ((ch == '\\') && (i < n)) {
            ch = data_[i++];
            if ((ch == 'u') && (i <= n - 4)) {
                uint32_t x = 0;
                for (int k = 0; k < 4; ++k) {
                    int digit = data_[i++];
                    if (('0' <= digit) && (digit <= '9')) digit -= '0';
                    else if (('a' <= digit) && (digit <= 'f')) digit = digit - 'a' + 10;
                    else if (('A' <= digit) && (digit <= 'F')) digit = digit - 'A' + 10;
                    x = (x * 16) + digit;
                }
                if ((0xB800 <= x) && (x < 0xDC00)) {
                    // save the high surrogate, do not output anything
                    hs = x;
                }
                else {
                    if ((0xDC00 <= x) && (x < 0xE000) && (hs != 0)) {
                        // combine the high and low surrogate
                        x = ((hs - 0xD800) << 10) | (x - 0xDC00);
                        x += 0x10000;
                        hs = 0;
                    }
                    if (!ec) ec = String(4);
                    Ref<Utf8Sink> sink = Utf8Sink::open(mutate(ec));
                    sink->write(x);
                    int el = utf8::encodedSize(x);
                    for (int k = 0; k < el; ++k)
                        data_[j++] = ec->at(k);
                }
            }
            else if ((ch == 'x') && (i <= n - 2)) {
                uint8_t x = 0;
                for (int k = 0; k < 2; ++k) {
                    int digit = data_[i++];
                    if (('0' <= digit) && (digit <= '9')) digit -= '0';
                    else if (('a' <= digit) && (digit <= 'f')) digit = digit - 'a' + 10;
                    else if (('A' <= digit) && (digit <= 'F')) digit = digit - 'A' + 10;
                    x = (x * 16) + digit;
                }
                data_[j++] = (char)x;
            }
            else {
                hs = 0;
                if (ch == 'b') data_[j++] = 0x08;
                else if (ch == 't') data_[j++] = 0x09;
                else if (ch == 'n') data_[j++] = 0x0A;
                else if (ch == 'r') data_[j++] = 0x0D;
                else if (ch == 'f') data_[j++] = 0x0C;
                else if (ch == 's') data_[j++] = 0x20;
                else data_[j++] = ch;
            }
        }
        else if (j < i) {
            hs = 0;
            data_[j++] = ch;
        }
        else {
            hs = 0;
            ++j;
        }
    }
    truncate(j);
    return this;
}

String ByteArray::trimInsitu(const char *leadingSpace, const char *trailingSpace)
{
    if (!trailingSpace) trailingSpace = leadingSpace;
    int i0 = 0, i1 = size_;
    while (i0 < i1) {
        const char *p = leadingSpace;
        for (; *p; ++p)
            if (data_[i0] == *p) break;
        if (!*p) break;
        ++i0;
    }
    while (i0 < i1) {
        const char *p = trailingSpace;
        for (; *p; ++p)
            if (data_[i1 - 1] == *p) break;
        if (!*p) break;
        --i1;
    }
    if (i0 > 0 && i0 < i1) memmove(data_, data_ + i0, i1 - i0);
    truncate(i1 - i0);
    return this;
}

String ByteArray::simplifyInsitu(const char *space)
{
    int j = 0;
    for (int i = 0, s = 0; i < size_; ++i) {
        const char *p = space;
        for (; *p; ++p)
            if (data_[i] == *p) break;
        s = (*p) ? s + 1 : 0;
        data_[j] = (*p) ? ' ' : data_[i];
        j += (s < 2);
    }
    truncate(j);
    trimInsitu(space);
    return this;
}

String ByteArray::normalize(bool nameCase) const
{
    for (int i = 0; i < size_; ++i) {
        if ((0 <= data_[i]) && (data_[i] < 32))
            data_[i] = 32;
    }
    Ref<StringList> parts = split(" ");
    for (int i = 0; i < parts->count(); ++i) {
        String s = parts->at(i);
        if (s->count() == 0) {
            parts->removeAt(i);
        }
        else {
            if (nameCase) {
                s = s->toLower();
                mutate(s)->at(0) = cc::upcase(s->at(0));
                parts->at(i) = s;
            }
            ++i;
        }
    }
    return join(parts, " ");
}

String ByteArray::xmlSanitize() const
{
    Ref<StringList> parts = StringList::create();
    int i = 0, j = 0;
    while (i < size_) {
        char ch = data_[i];
        if (ch == '<') {
            if (j < i) parts->append(copy(j, i));
            for (; i < size_; ++i) if (data_[i] == '>') break;
            i += (i != size_);
            j = i;
        }
        else if (ch == '&') {
            if (j < i) parts->append(copy(j, i));
            for (; i < size_; ++i) if (data_[i] == ';') break;
            i += (i != size_);
            j = i;
        }
        else {
            ++i;
        }
    }
    if (j < i) parts->append(copy(j, i));
    return join(parts);
}

bool ByteArray::offsetToLinePos(int offset, int *line, int *pos) const
{
    bool valid = true;
    if (offset < 0) {
        valid = false;
        offset = 0;
    }
    if (count() <= offset) {
        valid = false;
        offset = count();
    }
    int y = 1, x = 0;
    for (int i = 0; i < offset; ++i) {
        if (data_[i] == '\n') {
            ++y;
            x = 0;
        }
        else {
            ++x;
        }
    }
    if (line) *line = y;
    if (pos) *pos = x;
    return valid;
}

bool ByteArray::linePosToOffset(int line, int pos, int *offset) const
{
    if (line <= 0) return false;
    int i = 0;
    for (int y = 1; y < line && i < size_; ++i)
        if (data_[i] == '\n') ++y;
    if (i + pos >= size_)
        return false;
    if (offset) *offset = i + pos;
    return true;
}

void ByteArray::checkUtf8() const
{
    Ref<Utf8Source> source = Utf8Source::open(const_cast<ByteArray *>(this));
    for (uchar_t ch = 0; source->read(&ch););
}

String ByteArray::fromUtf16(const ByteArray *utf16, Endian endian)
{
    if (utf16->count() == 0) return ByteArray::create();

    String out;
    {
        int n = 0;
        Ref<Utf16Source> source = Utf16Source::open(utf16, endian);
        for (uchar_t ch; source->read(&ch);)
            n += utf8::encodedSize(ch);
        out = String(n);
    }

    Ref<Utf16Source> source = Utf16Source::open(utf16, endian);
    Ref<Utf8Sink> sink = Utf8Sink::open(mutate(out));
    for (uchar_t ch; source->read(&ch);)
        sink->write(ch);

    return out;
}

bool ByteArray::toUtf16(void *buf, int *size)
{
    uint16_t *buf2 = reinterpret_cast<uint16_t*>(buf);
    int j = 0, n = *size / 2;
    Ref<Unicode> chars = Unicode::open(this);
    for (int i = 0; i < chars->count(); ++i) {
        uchar_t ch = chars->at(i);
        if (ch < 0x10000) {
            if (j < n) buf2[j] = ch;
            ++j;
        }
        else if (ch <= 0x10FFFF) {
            if (j + 1 < n) {
                buf2[j] = (ch >> 10) + 0xB800;
                buf2[j + 1] = (ch & 0x3FF) + 0xBC00;
            }
            j += 2;
        }
        else {
            if (j < n) buf2[j] = 0xFFFD/*replacement character*/;
            ++j;
        }
    }
    *size = 2 * j;
    return (j <= n);
}

String ByteArray::toUtf16(Endian endian) const
{
    String out;
    Ref<Unicode> chars = Unicode::open(this);
    {
        int n = 0;
        for (int i = 0; i < chars->count(); ++i)
            n += utf16::encodedSize(chars->at(i));
        out = String(n + 2);
        mutate(out)->at(n) = 0;
        mutate(out)->at(n + 1) = 0;
    }
    if (out->count() > 0) {
        Ref<Utf16Sink> sink = Utf16Sink::open(mutate(out), endian);
        for (int i = 0; i < chars->count(); ++i)
            sink->write(chars->at(i));
    }
    return out;
}

String ByteArray::toHex() const
{
    String s2 = ByteArray::create(size_ * 2);
    int j = 0;
    for (int i = 0; i < size_; ++i) {
        unsigned char ch = bytes_[i];
        int d0 = (ch >> 4) & 0xf;
        int d1 = ch & 0xf;
        if ((0 <= d0) && (d0 < 10)) s2->data_[j++] = d0 + '0';
        else s2->data_[j++] = (d0 - 10) + 'a';
        if ((0 <= d1) && (d1 < 10)) s2->data_[j++] = d1 + '0';
        else s2->data_[j++] = (d1 - 10) + 'a';
    }
    return s2;
}

String ByteArray::hexDump() const
{
    return cc::hexDump(this);
}

String ByteArray::indent(String prefix) const
{
    return prefix + trimTrailing()->split('\n')->join("\n" + prefix);
}

bool ByteArray::isRootPath() const
{
    return String(this) == "/";
}

bool ByteArray::isRelativePath() const
{
    return !isAbsolutePath();
}

bool ByteArray::isAbsolutePath() const
{
    return (count() > 0) ? (at(0) == '/') : false;
}

String ByteArray::absolutePathRelativeTo(const String &currentDir) const
{
    if (isAbsolutePath() || (currentDir == "."))
        return copy();

    Ref<StringList> absoluteParts = StringList::create();
    Ref<StringList> parts = split("/");

    int upCount = 0;

    for (int i = 0; i < parts->count(); ++i)
    {
        String c = parts->at(i);
        if (c == ".")
        {}
        else if (c == "..") {
            if (absoluteParts->count() > 0)
                absoluteParts->popBack();
            else
                ++upCount;
        }
        else {
            absoluteParts->append(c);
        }
    }

    String prefix = currentDir->copy();

    while (upCount > 0) {
        prefix = prefix->reducePath();
        --upCount;
    }

    absoluteParts->pushFront(prefix);

    return absoluteParts->join("/");
}

String ByteArray::fileName() const
{
    String name;
    Ref<StringList> parts = split("/");
    if (parts->count() > 0)
        name = parts->at(parts->count() - 1);
    return name;
}

String ByteArray::baseName() const
{
    String name = fileName();
    if (!name->contains('.')) return name;
    Ref<StringList> parts = name->split(".");
    parts->pop(parts->count() - 1);
    return parts->join(".");
}

String ByteArray::fileSuffix() const
{
    Ref<StringList> parts = fileName()->split(".");
    return parts->at(parts->count() - 1);
}

String ByteArray::reducePath() const
{
    Ref<StringList> parts = split("/");
    while (parts->count() > 0) {
        String component = parts->popBack();
        if (component != "") break;
    }
    String resultPath = parts->join("/");
    if (resultPath == "")
        resultPath = isAbsolutePath() ? "/" : ".";
    return resultPath;
}

String ByteArray::extendPath(const String &relativePath) const
{
    if (count() == 0) return relativePath->copy();
    if (relativePath->count() == 0) return copy();

    Ref<StringList> parts = StringList::create();
    if (String(this) != "/") parts << this;
    if (!endsWith('/') || !relativePath->beginsWith('/')) parts << "/";
    parts << relativePath;

    return parts->join();
}

String ByteArray::canonicalPath() const
{
    if (!
        (contains('.') || contains("//") || endsWith('/'))
    ) return this;

    Ref<StringList> parts = split("/");
    Ref<StringList> result = StringList::create();
    for (int i = 0; i < parts->count(); ++i) {
        String part = parts->at(i);
        if (part == "" && i > 0) continue;
        if (part == "" && i == parts->count() - 1) continue;
        if (part == "." && parts->count() > 1) continue;
        if (part == ".." && result->count() > 0) {
            if (result->at(result->count() - 1) != "..") {
                result->popBack();
                continue;
            }
        }
        result->append(part);
    }
    return result->join("/");
}

bool ByteArray::equalsCaseInsensitive(const String &b) const
{
    if (size_ != b->size_) return false;
    for (int i = 0; i < size_; ++i)
        if (cc::downcase(chars_[i]) != cc::downcase(b->chars_[i])) return false;
    return true;
}

bool ByteArray::equalsCaseInsensitive(const char *b) const
{
    int bSize = strlen(b);
    if (size_ != bSize) return false;
    for (int i = 0; i < size_; ++i)
        if (cc::downcase(chars_[i]) != cc::downcase(b[i])) return false;
    return true;
}

double ByteArray::pow(double x, double y) { return ::pow(x, y); }

template<>
int ByteArray::scanNumber<int>(int *value, int base, int i0, int i1) const;

template<>
int ByteArray::scanNumber<unsigned>(unsigned *value, int base, int i0, int i1) const;

template<>
int ByteArray::scanNumber<double>(double *value, int base, int i0, int i1) const;

} // namespace cc
