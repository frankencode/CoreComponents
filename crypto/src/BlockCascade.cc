/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/crypto/BlockCascade>

namespace cc {
namespace crypto {

Ref<BlockCascade> BlockCascade::create(BlockCipher *cipher, const ByteArray *iv)
{
    return new BlockCascade(cipher, iv);
}

BlockCascade::BlockCascade(BlockCipher *cipher, const ByteArray *iv):
    BlockCipher(cipher->blockSize()),
    cipher_(cipher),
    s_(String::allocate(cipher->blockSize()))
{
    mutate(s_)->fill(0);
    if (iv) mutate(s_)->write(iv);
}

void BlockCascade::encode(const ByteArray *p, ByteArray *c)
{
    mutate(s_)->writeXor(p);
    cipher_->encode(s_, c);
    mutate(s_)->write(c);
}

void BlockCascade::decode(const ByteArray *c, ByteArray *p)
{
    cipher_->decode(c, p);
    p->writeXor(s_);
    mutate(s_)->write(c);
}

}} // namespace cc::crypto
