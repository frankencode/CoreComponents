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

HashMeter::Instance::Instance(const HashSink &hashSink, const Stream &stream):
    hashSink_{hashSink},
    stream_{stream}
{}

int HashMeter::Instance::read(CharArray *data)
{
    if (stream_) {
        int n = stream_->read(data);
        hashSink_->write(data->select(0, n));
        return n;
    }
    return 0;
}

void HashMeter::Instance::write(const CharArray *data)
{
    hashSink_->write(data);
    if (stream_) stream_->write(data);
}

void HashMeter::Instance::write(const StringList &parts)
{
    for (const String &part: parts)
        hashSink_->write(part);

    if (stream_) stream_->write(parts);
}

String HashMeter::Instance::finish()
{
    return hashSink_->finish();
}

}} // namespace cc::crypto
