/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXCRYPTO_CASCADEBLOCKCIPHER_H
#define FLUXCRYPTO_CASCADEBLOCKCIPHER_H

#include <flux/crypto/BlockCipher>

namespace flux {
namespace crypto {

class CascadeBlockCipher: public BlockCipher
{
public:
    CascadeBlockCipher(BlockCipher *cipher, ByteArray *iv);

    void encode(ByteArray *c, ByteArray *p);
    void decode(ByteArray *p, ByteArray *c);

private:
    Ref<BlockCipher> cipher_;
    Ref<ByteArray> s_;
};

}} // namespace flux::crypto

#endif // FLUXCRYPTO_CASCADEBLOCKCIPHER_H
