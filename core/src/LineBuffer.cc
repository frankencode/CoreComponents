/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/LineBuffer>

namespace cc {

Ref<LineBuffer> LineBuffer::open(Stream *stream, String prefix)
{
    return new LineBuffer(stream, prefix);
}

LineBuffer::LineBuffer(Stream *stream, String prefix):
    stream_(stream),
    prefix_(prefix),
    backlog_(StringList::create())
{}

String LineBuffer::prefix() const
{
    return prefix_;
}

int LineBuffer::read(ByteArray *data)
{
    return stream_->read(data);
}

void LineBuffer::write(const ByteArray *data)
{
    int i = 0, n = data->count();
    if (n == 0) return;
    while (i < n) {
        int i0 = i;
        i = data->find('\n', i);
        if (i < n) {
            ++i;
            backlog_->pushBack(data->copy(i0, i));
            i0 = i;
            flush();
        }
        else {
            backlog_->pushBack(data->copy(i0, n));
        }
    }
}

void LineBuffer::write(const StringList *parts)
{
    for (int i = 0, n = parts->count(); i < n; ++i)
        write(parts->at(i));
}

int LineBuffer::flush()
{
    String h = prefix();
    if (backlog_->count() == 0) return 0;
    if (h != "") backlog_->pushFront(h);
    String line = backlog_->join();
    stream_->write(line);
    backlog_ = StringList::create();
    return line->count();
}

} // namespace cc
