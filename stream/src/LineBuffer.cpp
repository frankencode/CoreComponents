/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stream/LineBuffer>

namespace flux {
namespace stream {

Ref<LineBuffer> LineBuffer::open(Stream *stream, String prefix)
{
    return new LineBuffer(stream, prefix);
}

LineBuffer::LineBuffer(Stream *stream, String prefix)
    : stream_(stream),
      prefix_(prefix),
      backlog_(StringList::create())
{}

String LineBuffer::prefix() const
{
    return prefix_;
}

bool LineBuffer::readyRead(double interval) const
{
    return stream_->readyRead(interval);
}

int LineBuffer::read(ByteArray *buf)
{
    return stream_->read(buf);
}

void LineBuffer::write(const ByteArray *buf)
{
    int i = 0, n = buf->count();
    while (i < n) {
        int i0 = i;
        i = buf->find('\n', i);
        if (i < n) {
            ++i;
            backlog_->pushBack(buf->copy(i0, i));
            i0 = i;
            String h = prefix();
            if (h != "") backlog_->pushFront(h);
            stream_->write(backlog_);
            backlog_ = StringList::create();
        }
        else {
            backlog_->pushBack(buf->copy(i0, n));
        }
    }
}

void LineBuffer::write(const StringList *parts)
{
    for (int i = 0, n = parts->count(); i < n; ++i)
        write(parts->at(i));
}

}} // namespace flux::stream
