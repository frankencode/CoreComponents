/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stream/TransferMeter>

namespace cc {
namespace stream {

Ref<TransferMeter> TransferMeter::open(Stream *stream) {
    return new TransferMeter(stream);
}

TransferMeter::TransferMeter(Stream *stream)
    : stream_(stream),
      totalRead_(0),
      totalWritten_(0)
{}

int TransferMeter::read(ByteArray *buf)
{
    int n = stream_->read(buf);
    totalRead_ += n;
    return n;
}

void TransferMeter::write(const ByteArray *buf)
{
    stream_->write(buf);
    totalWritten_ += buf->count();
}

void TransferMeter::write(const StringList *parts)
{
    stream_->write(parts);
    for (int i = 0, n = parts->count(); i < n; ++i)
        totalWritten_ += parts->at(i)->count();
}

}} // namespace cc::stream
