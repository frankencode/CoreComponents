/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/crypto/BlockCipher>

namespace cc {
namespace crypto {

/** \brief Rijndael Block Cipher according to the AES (FIPS-197)
  * \FIXME rename to AesCipher
  */
class AesCipher: public BlockCipher
{
public:
    enum { BlockSize = 16 };

    inline static Ref<AesCipher> create(const ByteArray *key) { return new AesCipher(key); }

    void encode(ByteArray *p, ByteArray *c);
    void decode(ByteArray *c, ByteArray *p);

private:
    AesCipher(const ByteArray *key);
    const int Nk_, Nr_;
    Ref<ByteArray> s_;
    Ref<ByteArray> w_;
};

}} // namespace cc::crypto
