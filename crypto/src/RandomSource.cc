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

Ref<RandomSource> RandomSource::open(const ByteArray *salt)
{
    return new RandomSource(salt);
}

RandomSource::RandomSource(const ByteArray *salt)
{
    Ref<ByteArray> key = ByteArray::allocate(16);
    Ref<ByteArray> iv = ByteArray::allocate(AesCipher::BlockSize);

    if (salt) {
        mutate(key)->write(salt);
        mutate(iv)->fill(0);
    }
    else {
        Ref<Stream> random = File::open("/dev/urandom");
        random->readSpan(mutate(key));
        random->readSpan(mutate(iv));
    }

    source_ =
        PseudoPad::open(
            BlockCascade::create(
                AesCipher::create(key),
                iv
            )
        );
}

int RandomSource::read(ByteArray *data)
{
    return source_->read(data);
}

}} // namespace cc::crypto
