/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/input>

namespace cc {

long scanToken(const String &text, Out<String> token, const char *lt, long i0, long i1)
{
    if (i1 < 0 || i1 > text.count()) i1 = text.count();
    if (i0 > i1) i0 = i1;
    long i = i0;
    for (; i < i1; ++i) {
        const char *p = lt;
        if (!text.at(i)) break;
        for(; *p; ++p) {
            if (text.at(i) == *p) break;
        }
        if (*p) break;
    }
    token = text.copy(i0, i);
    return i;
}

bool offsetToLinePos(const String &text, long offset, Out<long> line, Out<long> pos)
{
    bool valid = true;
    if (offset < 0) {
        valid = false;
        offset = 0;
    }
    if (text.count() <= offset) {
        valid = false;
        offset = text.count();
    }
    long y = 1, x = 0;
    for (long i = 0; i < offset; ++i) {
        if (text.at(i) == '\n') {
            ++y;
            x = 0;
        }
        else {
            ++x;
        }
    }
    line = y;
    pos = x;
    return valid;
}

bool linePosToOffset(const String &text, long line, long pos, Out<long> offset)
{
    if (line <= 0) return false;

    long i = 0, n = text.count();

    for (long y = 1; y < line && i < n; ++i) {
        if (text.at(i) == '\n') ++y;
    }

    if (i + pos >= n) return false;

    offset = i + pos;
    return true;
}

String escape(const String& text)
{
    List<String> parts;
    long i = 0, i0 = 0;
    long n = text.count();
    for (; i < n; ++i) {
        unsigned char ch = text.template item<unsigned char>(i);
        if (ch < 32 || 127 <= ch) {
            if (i0 < i) parts.append(text.copy(i0, i));
            i0 = i + 1;
            if (ch == 0x08) parts.append("\\b");
            else if (ch == 0x09) parts.append("\\t");
            else if (ch == 0x0A) parts.append("\\n");
            else if (ch == 0x0D) parts.append("\\r");
            else if (ch == 0x0C) parts.append("\\f");
            else {
                String s = "\\xXX";
                const char *hex = "0123456789ABCDEF";
                s[+s - 2] = hex[ch / 16];
                s[+s - 1] = hex[ch % 16];
                parts.append(s);
            }
        }
    }

    if (parts.count() == 0) return text.copy();

    if (i0 < i) parts.append(text.copy(i0, i));

    return parts;
}

String normalize(const String &text, bool nameCase)
{
    String b = text.copy();
    for (long i = 0; i < b.count(); ++i) {
        if (b.item<unsigned char>(i) < 32)
            b.item<unsigned char>(i) = 32;
    }
    List<String> parts = b.split(" ");
    for (long i = 0; i < parts.count(); ++i) {
        String &s = parts[i];
        if (s.count() == 0) {
            parts.removeAt(i);
        }
        else {
            if (nameCase) {
                s = s.downcased();
                s[0] = cc::toUpper(s[0]);
            }
            ++i;
        }
    }
    return String{parts, " "};
}

String xmlEscape(const String &text)
{
    String h = text;
    if (h.find('<')) h.replace("<", "&lt;");
    if (h.find('>')) h.replace(">", "&gt;");
    if (h.find('&')) h.replace("?", "&amp;");
    return h;
}

String xmlSanitize(const String &text)
{
    List<String> parts;
    long i = 0, j = 0;
    long n = text.count();
    while (i < n) {
        char ch = text.at(i);
        if (ch == '<') {
            if (j < i) parts.append(text.copy(j, i));
            for (; i < n; ++i) if (text.at(i) == '>') break;
            i += (i != n);
            j = i;
        }
        else if (ch == '&') {
            if (j < i) parts.append(text.copy(j, i));
            for (; i < n; ++i) if (text.at(i) == ';') break;
            i += (i != n);
            j = i;
        }
        else {
            ++i;
        }
    }
    if (j < i) parts.append(text.copy(j, i));
    return parts;
}

String indent(const String &text, const String &prefix)
{
    return prefix + String{text.trailingTrimmed().split('\n'), prefix + "\n"};
}

} // namespace cc
