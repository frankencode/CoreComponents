/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>
#include <cc/crypto/BlockCipher>

namespace cc {
namespace crypto {

/** \class CipherSink CipherSink.h cc/crypto/CipherSink
  * \brief Enciphering output stream
  */
class CipherSink: public Stream
{
public:
    static Ref<CipherSink> open(BlockCipher *cipher, Stream *sink, Stream *pad = 0);

    virtual void write(const ByteArray *data);

private:
    CipherSink(BlockCipher *cipher, Stream *sink, Stream *pad);
    ~CipherSink();

    Ref<BlockCipher> cipher_;
    Ref<Stream> sink_;
    Ref<Stream> pad_;
    Ref<ByteArray> block_;
    Ref<ByteArray> pending_;
};

}} // namespace cc::crypto
