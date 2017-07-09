/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>
#include <cc/HashSink>

namespace cc {
namespace crypto {

/** \class HashMeter HashMeter.h cc/crypto/HashMeter
  * \brief Hash sum computing output stream
  * \see cc::stream::TransferMeter
  */
class HashMeter: public Stream
{
public:
    static Ref<HashMeter> open(HashSink *hashSum, Stream *stream = 0);

    int read(ByteArray *data) override;
    void write(const ByteArray *data) override;
    void write(const StringList *parts) override;

    Ref<ByteArray> finish();

private:
    HashMeter(HashSink *hashSum, Stream *stream);

    Ref<HashSink> hashSum_;
    Ref<Stream> stream_;
};

}} // namespace cc::crypto
