/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/types>
#include <cc/Format>
#include <cc/Stream>

namespace cc {

int Stream::read(CharArray *)
{
    return 0;
}

void Stream::write(const CharArray *)
{}

void Stream::write(const StringList *parts)
{
    for (int i = 0, n = parts->count(); i < n; ++i)
        write(parts->at(i));
}

void Stream::write(const Format &format)
{
    write(Format::toStringList(format));
}

class WrappedChunk: public CharArray {
public:
    WrappedChunk(const void *data, int size):
        CharArray((const char *)data, size, CharArray::doNothing)
    {
        decRefCount();
    }
};

void Stream::write(const void *data, int size)
{
    WrappedChunk chunk(data, size);
    write(&chunk);
}

off_t Stream::transferSpanTo(off_t count, Stream *sink, CharArray *buffer)
{
    if (count == 0) return 0;

    off_t total = 0;
    String h;
    if (!buffer) {
        h = String::allocate((0 < count && count < 0x4000) ? count : 0x4000);
        buffer = mutate(h);
    }

    while (true) {
        int n = read(buffer);
        if (n == 0) break;
        if (sink) sink->write(buffer->select(0, n));
        total += n;
        if (count > 0) {
            count -= n;
            if (count == 0) break;
            if (count < buffer->count()) buffer->truncate(count);
        }
    }

    return total;
}

int Stream::readSpan(CharArray *data)
{
    const int w = data->count();
    int m = 0;
    while (m < w) {
        int n = read(mutate(data->select(m, w)));
        if (n == 0) break;
        m += n;
    }
    return m;
}

String Stream::readSpan(int count)
{
    if (count == 0) return String();
    if (count < 0) return readAll();
    String s(count);
    readSpan(mutate(s));
    return s;
}

String Stream::readAll(CharArray *buffer)
{
    String data = buffer;
    if (!data) data = String::allocate(0x4000);
    Ref<StringList> parts = StringList::create();
    while (true) {
        int n = read(mutate(data));
        if (n == 0) break;
        parts->append(data->copy(0, n));
    }
    return parts->join();
}

} // namespace cc
