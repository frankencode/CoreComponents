/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/crypto/RandomSource>
#include <cc/crypto/PseudoPad>
#include <cc/crypto/AesCipher>
#include <cc/crypto/BlockCascade>
#include <cc/File>

namespace cc {
namespace crypto {

RandomSource::RandomSource(const CharArray *salt)
{
    String key = String::allocate(16);
    String iv = String::allocate(AesCipher::BlockSize);

    if (salt) {
        mutate(key)->write(salt);
        mutate(iv)->fill(0);
    }
    else {
        File file{"/dev/urandom"};
        file->readSpan(mutate(key));
        file->readSpan(mutate(iv));
    }

    source_ =
        PseudoPad{
            BlockCascade{
                AesCipher{key},
                iv
            }
        };
}

}} // namespace cc::crypto
