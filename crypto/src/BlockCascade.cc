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

Ref<BlockCascade> BlockCascade::create(BlockCipher *cipher, const CharArray *iv)
{
    return new BlockCascade(cipher, iv);
}

BlockCascade::BlockCascade(BlockCipher *cipher, const CharArray *iv):
    BlockCipher(cipher->blockSize()),
    cipher_(cipher),
    s_(String::allocate(cipher->blockSize()))
{
    mutate(s_)->fill(0);
    if (iv) mutate(s_)->write(iv);
}

void BlockCascade::encode(const CharArray *p, CharArray *c)
{
    mutate(s_)->writeXor(p);
    cipher_->encode(s_, c);
    mutate(s_)->write(c);
}

void BlockCascade::decode(const CharArray *c, CharArray *p)
{
    cipher_->decode(c, p);
    p->writeXor(s_);
    mutate(s_)->write(c);
}

}} // namespace cc::crypto
