/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/ByteArray>

namespace cc {
namespace crypto {

/** \class BlockCipher BlockCipher.h cc/crypto/BlockCipher
  * \brief Cryptographic block cipher
  */
class BlockCipher: public Object
{
public:
    /// Block size in bytes
    inline int blockSize() const { return blockSize_; }

    /// Encipher a plaintext p into a ciphertext c
    virtual void encode(const ByteArray *p, ByteArray *c) = 0;

    /// Decipher a ciphertext c into a plaintext p
    virtual void decode(const ByteArray *c, ByteArray *p) = 0;

protected:
    BlockCipher(int blockSize): blockSize_(blockSize) {}

private:
    int blockSize_;
};

}} // namespace cc::crypto
