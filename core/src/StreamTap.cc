/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/StreamTap>

namespace cc {

Ref<StreamTap> StreamTap::open(Stream *stream, Stream *inputTap, Stream *outputTap)
{
    return new StreamTap(stream, inputTap, outputTap);
}

StreamTap::StreamTap(Stream *stream, Stream *inputTap, Stream *outputTap):
    stream_(stream),
    inputTap_(inputTap),
    outputTap_(outputTap)
{}

int StreamTap::read(CharArray *data)
{
    int n = stream_->read(data);
    if (inputTap_) inputTap_->write(data->select(0, n));
    return n;
}

void StreamTap::write(const CharArray *data)
{
    if (outputTap_) outputTap_->write(data);
    stream_->write(data);
}

void StreamTap::write(const StringList *parts)
{
    if (outputTap_) outputTap_->write(parts);
    stream_->write(parts);
}

} // namespace cc
