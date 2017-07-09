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

/** \class CipherSource CipherSource.h cc/crypto/CipherSource
  * \brief Deciphering input stream
  */
class CipherSource: public Stream
{
public:
    /** Open a new deciphering input stream
      * \param cipher cipher to use for deciphering the ciphertext
      * \param source source to read the ciphertext from
      * \return new object instance
      */
    static Ref<CipherSource> open(BlockCipher *cipher, Stream *source);

    virtual int read(ByteArray *data) override;

private:
    CipherSource(BlockCipher *cipher, Stream *source);

    Ref<BlockCipher> cipher_;
    Ref<Stream> source_;
    Ref<ByteArray> block_;
    Ref<ByteArray> buffer_; // decoding buffer
    Ref<ByteArray> pending_; // undecoded pending bytes
    int j_, m_; // consumption index and fill mark
};

}} // namespace cc::crypto
