/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/LineBuffer>

namespace cc {

Ref<LineBuffer> LineBuffer::open(Stream *stream, const string &prefix)
{
    return new LineBuffer{stream, prefix};
}

LineBuffer::LineBuffer(Stream *stream, const string &prefix):
    stream_{stream},
    prefix_{prefix},
    backlog_{StringList::create()}
{}

void LineBuffer::writeLine(const CharArray *data)
{
    if (stream_) stream_->write(data);
}

string LineBuffer::prefix() const
{
    return prefix_;
}

int LineBuffer::read(CharArray *data)
{
    return (stream_) ? stream_->read(data) : 0;
}

void LineBuffer::write(const CharArray *data)
{
    int i = 0, n = data->count();
    if (n == 0) return;
    while (i < n) {
        int i0 = i;
        if (data->find('\n', &i)) {
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
    string h = prefix();
    if (backlog_->count() == 0) return 0;
    if (h != "") backlog_->pushFront(h);
    string line = backlog_->join();
    writeLine(line);
    backlog_ = StringList::create();
    return line->count();
}

} // namespace cc
