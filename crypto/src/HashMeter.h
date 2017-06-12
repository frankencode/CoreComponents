/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>
#include <cc/HashSum>

namespace cc {
namespace crypto {

/** \class HashMeter HashMeter.h cc/crypto/HashMeter
  * \brief Hash sum computing output stream
  * \see cc::stream::TransferMeter
  */
class HashMeter: public Stream
{
public:
    static Ref<HashMeter> open(HashSum *hashSum, Stream *stream = 0);

    int read(ByteArray *data) override;
    void write(const ByteArray *data) override;
    void write(const StringList *parts) override;

    Ref<ByteArray> finish();

private:
    HashMeter(HashSum *hashSum, Stream *stream);

    Ref<HashSum> hashSum_;
    Ref<Stream> stream_;
};

}} // namespace cc::crypto
