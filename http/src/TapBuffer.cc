/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Thread>
#include <cc/str>
#include <cc/http/TapBuffer>

namespace cc {
namespace http {

Ref<TapBuffer> TapBuffer::open(Stream *stream, String prefix)
{
    return new TapBuffer(stream, prefix);
}

TapBuffer::TapBuffer(Stream *stream, String prefix):
    LineBuffer(stream),
    prefix_(prefix)
{}

String TapBuffer::prefix() const
{
    return "(" + inum(uint64_t(Thread::self()->id()), 62) + ") " + prefix_;
}

void TapBuffer::write(const CharArray *buf)
{
    Ref<StringList> parts;
    int i = 0, i0 = 0;
    for (int n = buf->count(); i < n; ++i) {
        uint8_t ch = buf->byteAt(i);
        if (ch <= 31 || 127 <= ch) {
            if (!parts) parts = StringList::create();
            if (i0 < i) parts->append(buf->copy(i0, i));
            i0 = i + 1;
            if (ch == 0x08) parts->append("\\b");
            else if (ch == 0x09) parts->append("\\t");
            else if (ch == 0x0A) parts->append("\\n\n");
            else if (ch == 0x0D) parts->append("\\r");
            else if (ch == 0x0C) parts->append("\\f");
            else {
                String s = "\\xXX";
                const char *hex = "0123456789ABCDEF";
                mutate(s)->at(s->count() - 2) = hex[ch / 16];
                mutate(s)->at(s->count() - 1) = hex[ch % 16];
                parts->append(s);
                if (ch == 0) parts->append("\n");
            }
        }
    }
    if (parts) {
        if (i0 < i) parts->append(buf->copy(i0, i));
        LineBuffer::write(parts->join());
    }
    else
        LineBuffer::write(buf);
}

}} // namespace cc::http
