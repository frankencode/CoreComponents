/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/AesEntropySource>
#include <cc/AesBlockCipher>
#include <cc/CbcBlockCipher>
#include <cc/File>
#include <algorithm>

namespace cc {

namespace aes {

BlockCipher createEntropyCipher(const Bytes &salt)
{
    Bytes key = Bytes::allocate(16 + (salt.count() > 16 ? 8 : 0) + (salt.count() > 24 ? 8 : 0));
    Bytes start = Bytes::allocate(AesBlockCipher::BlockSize);

    if (salt) {
        key.fill(0);
        start.fill(0);
        salt.copyRangeTo(0, std::min(key.count(), salt.count()), &key);
    }
    else {
        File file{"/dev/urandom"};
        file.readSpan(&key);
        file.readSpan(&start);
    }

    return CbcBlockCipher{ AesBlockCipher{key}, start };
}

} // namespace aes

AesEntropySource::AesEntropySource(const Bytes &salt):
    PseudoPad{aes::createEntropyCipher(salt)}
{}

} // namespace cc
