/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/LineSource>

namespace cc {

Ref<LineSource> LineSource::open(const ByteArray *buffer) {
    return new LineSource(0, const_cast<ByteArray *>(buffer));
}

Ref<LineSource> LineSource::open(Stream *stream, ByteArray *buffer) {
    return new LineSource(stream, buffer);
}

LineSource::LineSource(Stream *stream, ByteArray *buffer):
    stream_(stream),
    buffer_(buffer),
    eoi_(false),
    i_(0), n_(0)
{
    if (!buffer_) buffer_ = ByteArray::allocate(0x1000);
}

bool LineSource::read(String *line)
{
    if (eoi_) {
        *line = String();
        return false;
    }

    Ref<StringList> backlog;

    while (true) {
        if (i_ < n_) {
            int i0 = i_;
            i_ = findEol(buffer_, n_, i_);
            if (i_ < n_) {
                if (backlog) {
                    backlog->append(buffer_->copy(i0, i_));
                    *line = backlog->join();
                }
                else {
                    *line = buffer_->copy(i0, i_);
                }
                i_ = skipEol(buffer_, n_, i_);
                return true;
            }
            if (!backlog)
                backlog = StringList::create();
            backlog->append(buffer_->copy(i0, i_));
        }

        if (!stream_) break;

        n_ = stream_->read(buffer_);
        if (n_ == 0) break;
        i_ = 0;
    }

    eoi_ = true;
    if (backlog) {
        *line = backlog->join();
        return true;
    }
    *line = String();
    return false;
}

String LineSource::readLine()
{
    String s;
    read(&s);
    return s;
}

String LineSource::pendingData() const
{
    if (eoi_) return String();
    return buffer_->copy(i_, n_);
}

int LineSource::findEol(const ByteArray *buffer, int n, int i)
{
    for (; i < n; ++i) {
        char ch = buffer->at(i);
        if (ch == '\n' || ch == '\r' || ch == '\0')
            break;
    }
    return i;
}

int LineSource::skipEol(const ByteArray *buffer, int n, int i)
{
    if (i < n) if (buffer->at(i) == '\r') ++i;
    if (i < n) if (buffer->at(i) == '\n') ++i;
    if (i < n) if (buffer->at(i) == '\0') ++i;
    return i;
}

} // namespace cc
