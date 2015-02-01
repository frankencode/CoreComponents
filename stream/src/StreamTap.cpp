/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stream/StreamTap>

namespace flux {
namespace stream {

Ref<StreamTap> StreamTap::open(Stream *stream, Stream *inputTap, Stream *outputTap)
{
    return new StreamTap(stream, inputTap, outputTap);
}

StreamTap::StreamTap(Stream *stream, Stream *inputTap, Stream *outputTap)
    : stream_(stream),
      inputTap_(inputTap),
      outputTap_(outputTap)
{}

bool StreamTap::readyRead(double interval) const
{
    return stream_->readyRead(interval);
}

int StreamTap::read(ByteArray *buf)
{
    int n = stream_->read(buf);
    inputTap_->write(ByteRange(buf, 0, n));
    return n;
}

void StreamTap::write(const ByteArray *buf)
{
    outputTap_->write(buf);
    stream_->write(buf);
}

void StreamTap::write(const StringList *parts)
{
    outputTap_->write(parts);
    stream_->write(parts);
}

}} // namespace flux::stream
