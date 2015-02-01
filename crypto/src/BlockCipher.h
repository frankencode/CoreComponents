/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXCRYPTO_BLOCKCIPHER_H
#define FLUXCRYPTO_BLOCKCIPHER_H

#include <flux/ByteArray>

namespace flux {
namespace crypto {

/** \brief Cryptographic block cipher
  */
class BlockCipher: public Object
{
public:
    inline int blockSize() const { return blockSize_; }

    virtual void encode(ByteArray *c, ByteArray *p) = 0;
    virtual void decode(ByteArray *c, ByteArray *p) = 0;

protected:
    BlockCipher(int blockSize): blockSize_(blockSize) {}

private:
    int blockSize_;
};

}} // namespace flux::crypto

#endif // FLUXCRYPTO_BLOCKCIPHER_H
