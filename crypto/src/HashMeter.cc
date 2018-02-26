/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/crypto/HashMeter>

namespace cc {
namespace crypto {

Ref<HashMeter> HashMeter::open(HashSink *hashSink, Stream *stream )
{
    return new HashMeter(hashSink, stream);
}

HashMeter::HashMeter(HashSink *hashSink, Stream *stream):
    hashSink_(hashSink),
    stream_(stream)
{}

int HashMeter::read(ByteArray *data)
{
    if (stream_) {
        int n = stream_->read(data);
        hashSink_->write(data->select(0, n));
        return n;
    }
    return 0;
}

void HashMeter::write(const ByteArray *data)
{
    hashSink_->write(data);
    if (stream_) stream_->write(data);
}

void HashMeter::write(const StringList *parts)
{
    for (int i = 0, n = parts->count(); i < n; ++i)
        hashSink_->write(parts->at(i));
    if (stream_) stream_->write(parts);
}

String HashMeter::finish()
{
    return hashSink_->finish();
}

}} // namespace cc::crypto
