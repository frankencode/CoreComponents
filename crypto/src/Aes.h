/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXCRYPTO_AES_H
#define FLUXCRYPTO_AES_H

#include <flux/crypto/BlockCipher>

namespace flux {
namespace crypto {

/** \brief Rijndael Block Cipher according to the AES (FIPS-197)
  */
class Aes: public BlockCipher
{
public:
    inline static Ref<Aes> create(ByteArray *key) { return new Aes(key); }

    void encode(ByteArray *p, ByteArray *c);
    void decode(ByteArray *c, ByteArray *p);

private:
    Aes(ByteArray *key);
    const int Nk_, Nr_;
    Ref<ByteArray> s_;
    Ref<ByteArray> w_;
};

}} // namespace flux::crypto

#endif // FLUXCRYPTO_AES_H
