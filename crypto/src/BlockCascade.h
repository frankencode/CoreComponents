/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/crypto/BlockCipher>

namespace cc {
namespace crypto {

/** \class BlockCascade BlockCascade.h cc/crypto/BlockCascade
  * \brief CBC frontend
  *
  * The BlockCascade allows to run any block cipher in CBC mode.
  */
class BlockCascade: public BlockCipher
{
public:
    static Ref<BlockCascade> create(BlockCipher *cipher, ByteArray *iv = 0);

    void encode(const ByteArray *c, ByteArray *p);
    void decode(const ByteArray *p, ByteArray *c);

private:
    BlockCascade(BlockCipher *cipher, ByteArray *iv);

    Ref<BlockCipher> cipher_;
    Ref<ByteArray> s_;
};

}} // namespace cc::crypto
