/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/crypto/HashMeter>

namespace cc {
namespace crypto {

Ref<HashMeter> HashMeter::open(HashSum *hashSum, Stream *stream )
{
    return new HashMeter(hashSum, stream);
}

HashMeter::HashMeter(HashSum *hashSum, Stream *stream):
    hashSum_(hashSum),
    stream_(stream)
{}

int HashMeter::read(ByteArray *data)
{
    if (stream_) {
        int n = stream_->read(data);
        hashSum_->write(data->select(0, n));
        return n;
    }
    return 0;
}

void HashMeter::write(const ByteArray *data)
{
    hashSum_->write(data);
    if (stream_) stream_->write(data);
}

void HashMeter::write(const StringList *parts)
{
    for (int i = 0, n = parts->count(); i < n; ++i)
        hashSum_->write(parts->at(i));
    if (stream_) stream_->write(parts);
}

Ref<ByteArray> HashMeter::finish()
{
    return hashSum_->finish();
}

}} // namespace cc::crypto
