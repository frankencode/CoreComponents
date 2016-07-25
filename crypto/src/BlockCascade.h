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

/** \brief CBC frontend
  *
  * The BlockCascade allows to run any block cipher in CBC mode.
  */
class BlockCascade: public BlockCipher
{
public:
    static Ref<BlockCascade> create(BlockCipher *cipher, ByteArray *iv = 0);

    void encode(ByteArray *c, ByteArray *p);
    void decode(ByteArray *p, ByteArray *c);

private:
    BlockCascade(BlockCipher *cipher, ByteArray *iv);

    Ref<BlockCipher> cipher_;
    Ref<ByteArray> s_;
};

}} // namespace cc::crypto
