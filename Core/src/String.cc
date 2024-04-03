/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/String>
#include <cc/Utf8Sink>
#include <cc/Casefree>

namespace cc {

String String::alignedLeft(int w, char blank) const
{
    return (count() > w) ? *this : *this + String::allocate(w - count(), blank);
}

String String::alignedRight(int w, char blank) const
{
    return (count() > w) ? *this : String::allocate(w - count(), blank) + *this;
}

bool String::find(const char *b, long bn, InOut<long> i0) const
{
    assert(0 <= i0);

    if (bn < 0) bn = len(b);

    long i = i0;
    long n = count();
    const char *a = chars();

    if (n <= i || bn == 0) {
        i0 = n;
        return false;
    }

    for (long j = i, k = 0; j < n;) {
        if (a[j++] == b[k]) {
            ++k;
            if (k == bn) {
                i0 = j - k;
                return true;
            }
        }
        else {
            k = 0;
        }
    }

    i0 = n;
    return false;
}

void String::replace(const char *b, const char *s, long bn, long sn)
{
    if (!b || !s) return;

    if (bn < sn) {
        if (find(b)) {
            (*this) = String{split(b, bn), s, sn};
        }
    }
    else if (bn > 0) {
        long i = 0, j = 0, k = 0, n = me().count;
        char *a = chars();
        while (i < n) {
            char ch = a[i++];
            if (j < i) a[j++] = ch;
            if (ch == b[k]) {
                ++k;
                if (k == bn) {
                    j -= bn;
                    for (k = 0; k < sn; ++k)
                        a[j++] = s[k];
                    k = 0;
                }
            }
            else {
                k = 0;
            }
        }
        truncate(j);
    }
}

List<String> String::split(char sep) const
{
    List<String> parts;
    long n = me().count;
    for (long i = 0; i < n;) {
        long j = i;
        find(sep, &j);
        parts << copy(i, j);
        i = j + 1;
        if (i == n) {
            parts << String{};
            break;
        }
    }
    return parts;
}

List<String> String::split(const char *sep, long sepLength) const
{
    List<String> parts;
    long i0 = 0;
    long n = me().count;
    while (i0 < n) {
        long i1 = i0;
        if (!find(sep, &i1)) break;
        parts << copy(i0, i1);
        i0 = i1 + sepLength;
    }
    if (i0 < n)
        parts << copy(i0, n);
    else
        parts << String{};
    return parts;
}

List<String> String::breakUp(long m) const
{
    List<String> parts;
    long i0 = 0;
    long n = me().count;
    while (i0 < n) {
        long i1 = i0 + m;
        if (i1 > n) i1 = n;
        parts << copy(i0, i1);
        i0 += m;
    }
    return parts;
}

String String::times(long n) const
{
    if (me().count == 0) return String{};

    String result = String::allocate(me().count * n);
    long offset = 0;
    for (; n > 0; --n) {
        std::memcpy(result.chars() + offset, chars(), count() * sizeof(char));
        offset += me().count;
    }

    return result;
}

String String::paste(long i0, long i1, const String &text) const
{
    assert(0 <= i0 && i0 <= i1 && i1 <= count());
    if (i0 == i1 && text.count() == 0) return copy();

    String result = String::allocate(i0 + text.count() + count() - i1);

    if (0 < i0) std::memcpy(result.chars(), chars(), i0 * sizeof(char));
    if (0 < text.count()) std::memcpy(result.chars() + i0, text.chars(), text.count() * sizeof(char));
    if (i1 < count()) std::memcpy(result.chars() + i0 + text.count(), chars() + i1, (count() - i1) * sizeof(char));

    return result;
}

void String::trim(const char *ls, const char *ts)
{
    if (!ts) ts = ls;
    long i0 = 0, i1 = count();
    char *a = chars();
    while (i0 < i1) {
        const char *p = ls;
        for (; *p; ++p)
            if (a[i0] == *p) break;
        if (!*p) break;
        ++i0;
    }
    while (i0 < i1) {
        const char *p = ts;
        for (; *p; ++p)
            if (a[i1 - 1] == *p) break;
        if (!*p) break;
        --i1;
    }
    if (i0 > 0 && i0 < i1) std::memmove(a, a + i0, i1 - i0);
    truncate(i1 - i0);
}

void String::simplify(const char *ws)
{
    long j = 0;
    long n = count();
    char *a = chars();
    for (long i = 0, s = 0; i < n; ++i) {
        const char *p = ws;
        for (; *p; ++p)
            if (a[i] == *p) break;
        s = (*p) ? s + 1 : 0;
        a[j] = (*p) ? ' ' : a[i];
        j += (s < 2);
    }
    truncate(j);
    trim(ws);
}

String String::escaped() const
{
    List<String> parts;
    int i = 0, i0 = 0, n = count();
    for (; i < n; ++i) {
        uint8_t ch = item<uint8_t>(i);
        if (ch < 32 || 127 <= ch) {
            if (i0 < i) parts.append(copy(i0, i));
            i0 = i + 1;
            if (ch == 0x08) parts.append("\\b");
            else if (ch == 0x09) parts.append("\\t");
            else if (ch == 0x0A) parts.append("\\n");
            else if (ch == 0x0D) parts.append("\\r");
            else if (ch == 0x0C) parts.append("\\f");
            else {
                String s = "\\xXX";
                const char *hex = "0123456789ABCDEF";
                s.at(s.count() - 2) = hex[ch / 16];
                s.at(s.count() - 1) = hex[ch % 16];
                parts.append(s);
            }
        }
    }
    if (parts.count() == 0) return copy();

    if (i0 < i) parts.append(copy(i0, i));

    return parts;
}

void String::expand()
{
    if (!find('\\')) return;
    int j = 0;
    uint32_t hs = 0; // high surrogate, saved
    String ec; // buffer for encoded character
    for (int i = 0, n = count(); i < n;) {
        char ch = at(i++);
        if ((ch == '\\') && (i < n)) {
            ch = at(i++);
            if ((ch == 'u') && (i <= n - 4)) {
                uint32_t x = 0;
                for (int k = 0; k < 4; ++k) {
                    int digit = at(i++);
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
                    if (ec.count() == 0) ec = String::allocate(4);
                    Utf8Sink sink{ec};
                    sink.write(x);
                    const int el = sink.currentOffset();
                    for (int k = 0; k < el; ++k)
                        at(j++) = ec.at(k);
                }
            }
            else if ((ch == 'x') && (i <= n - 2)) {
                uint8_t x = 0;
                for (int k = 0; k < 2; ++k) {
                    int digit = at(i++);
                    if (('0' <= digit) && (digit <= '9')) digit -= '0';
                    else if (('a' <= digit) && (digit <= 'f')) digit = digit - 'a' + 10;
                    else if (('A' <= digit) && (digit <= 'F')) digit = digit - 'A' + 10;
                    x = (x * 16) + digit;
                }
                at(j++) = static_cast<char>(x);
            }
            else if (('0' <= ch) && (ch <= '9') && (i <= n - 1)) {
                uint8_t x = ch - '0';
                x <<= 4;
                x |= at(i++) - '0';
                at(j++) = static_cast<char>(x);
            }
            else {
                hs = 0;
                if (ch == 'b') at(j++) = 0x08;
                else if (ch == 't') at(j++) = 0x09;
                else if (ch == 'n') at(j++) = 0x0A;
                else if (ch == 'r') at(j++) = 0x0D;
                else if (ch == 'f') at(j++) = 0x0C;
                else if (ch == 's') at(j++) = 0x20;
                else at(j++) = ch;
            }
        }
        else if (j < i) {
            hs = 0;
            at(j++) = ch;
        }
        else {
            hs = 0;
            ++j;
        }
    }
    truncate(j);
}

String String::indented(const String &prefix)
{
    return prefix + String{trailingTrimmed().split('\n'), "\n" + prefix};
}

bool String::toBool(Out<bool> ok) const
{
    bool value = false;
    ok = readBool(&value);
    return value;
}

int String::toInt(Out<bool> ok) const
{
    int okCount = 0;
    int value = cc::readNumber<int>(*this, &okCount);
    ok = (okCount == 1);
    return value;
}

long String::toLong(Out<bool> ok) const
{
    int okCount = 0;
    long value = cc::readNumber<long>(*this, &okCount);
    ok = (okCount == 1);
    return value;
}

double String::toDouble(Out<bool> ok) const
{
    int okCount = 0;
    double value = cc::readNumber<double>(*this, &okCount);
    ok = (okCount == 1);
    return value;
}

bool String::readBool(Out<bool> value) const
{
    bool ok = false;

    if (
        Casefree<String>(*this) == "on" ||
        Casefree<String>(*this) == "true" ||
        Casefree<String>(*this) == "1"
    ) {
        value = true;
        ok = true;
    }
    else if (
        Casefree<String>(*this) == "off" ||
        Casefree<String>(*this) == "false" ||
        Casefree<String>(*this) == "0"
    ) {
        value = false;
        ok = true;
    }

    return ok;
}

bool String::isHerePath() const
{
    const char *s = chars();
    const long n = count();
    if (n == 0) return false;
    if (s[0] != '.') return false;
    int m = 0;
    for (long i = 0; i < n; ++i) {
        char ch = s[i];
        if (ch == '.') {
            ++m;
            if (m == 2) return false;
        }
        else if (ch == '/') {
            m = 0;
        }
        else
            return false;
    }
    return true;
}

String String::absolutePathRelativeTo(const String &currentDir) const
{
    if (isAbsolutePath() || (currentDir == "."))
        return copy();

    List<String> absoluteParts;
    List<String> parts = split('/');

    int upCount = 0;

    for (const String &c: parts)
    {
        if (c == ".")
        {}
        else if (c == "..") {
            if (absoluteParts.count() > 0)
                absoluteParts.popBack();
            else
                ++upCount;
        }
        else {
            absoluteParts.append(c);
        }
    }

    String prefix = currentDir.copy();

    while (upCount > 0) {
        prefix = prefix.cdUp();
        --upCount;
    }

    absoluteParts.pushFront(prefix);

    return absoluteParts.join('/');
}

String String::fileName() const
{
    long i = count();
    while (i > 0) {
        --i;
        if (at(i) == '/') {
            ++i;
            break;
        }
    }
    return copy(i, count());
}

String String::baseName() const
{
    String name = fileName();
    long i = 0;
    if (name.find('.', &i)) name.truncate(i);
    return name;
}

String String::fileSuffix() const
{
    String name = fileName();
    long i = 0;
    return name.find('.', &i) ? name.copy(i + 1, name.count()) : String{};
}

String String::longBaseName() const
{
    String name = fileName();
    long j = name.count();
    for(long i = 0; name.find('.', &i); ++i) j = i;
    if (j < name.count()) name.truncate(j);
    return name;
}

String String::shortFileSuffix() const
{
    String suffix;
    List<String> parts = split('.');
    if (parts.count() > 1) suffix = parts.last();
    return suffix;
}

String String::sansFileSuffix() const
{
    long j = 0;
    for (long i = 0; find('/', &i);) j = ++i;
    if (j >= count()) return *this;
    if (find('.', &j)) return copy(0, j);
    return *this;
}

String String::operator/(const String &relativePath) const
{
    if (count() == 0/* || isHerePath()*/) return relativePath.copy();
    if (relativePath.count() == 0) return copy();

    List<String> parts;
    if (!isRootPath()) parts << *this;
    if (!endsWith('/') || !relativePath.startsWith('/')) parts << "/";
    parts << relativePath;

    return parts;
}

String String::cd(const String &target) const
{
    if (target == "..") return cdUp();
    if (target == ".") return copy();

    if ((*this) == "" || (*this) == "." || target.isAbsolutePath())
        return target.copy();

    List<String> absoluteParts;
    List<String> parts = target.split('/');

    int upCount = 0;

    for (const String &c: parts)
    {
        if (c == ".")
        {}
        else if (c == "..") {
            if (absoluteParts.count() > 0)
                absoluteParts.popBack();
            else
                ++upCount;
        }
        else {
            absoluteParts.append(c);
        }
    }

    String prefix = *this;
    while (upCount > 0) {
        prefix = prefix.cdUp();
        --upCount;
    }

    absoluteParts.pushFront(prefix);

    return String{absoluteParts, '/'};
}

String String::cdUp() const
{
    List<String> parts = split('/');
    while (parts.count() > 0) {
        String component = parts.last();
        parts.popBack();
        if (component != "") break;
    }

    String resultPath{parts, '/'};
    if (resultPath == "")
        resultPath = isAbsolutePath() ? "/" : ".";

    return resultPath;
}

String String::canonicalPath() const
{
    if (!(
        endsWith('/') || find("//") || find("./") || endsWith("/.")
    )) return copy();

    List<String> parts = split('/');
    List<String> result;
    for (Locator pos = parts.head(); pos; ++pos) {
        String part = parts.at(pos);
        if (part == "" && +pos > 0) continue;
        if (part == "" && +pos == parts.count() - 1) continue;
        if (part == "." && parts.count() > 1) continue;
        if (part == ".." && result.count() > 0) {
            if (result.last() != "..") {
                result.popBack();
                continue;
            }
        }
        result.append(part);
    }
    return String{result, '/'};
}

} // namespace cc
