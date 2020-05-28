/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/crypto/HashMacSink>

namespace cc {
namespace crypto {

Ref<HashMacSink> HashMacSink::open(const CryptoHashSink *hashSink, const CharArray *key)
{
    return new HashMacSink{hashSink, key};
}

Ref<CryptoHashSink> HashMacSink::prepareHashSink(const CryptoHashSink *hashSink, const CharArray *key, uint8_t blind)
{
    auto preparedKey = String::allocate(hashSink->blockSize());
    mutate(preparedKey)->fill(0);
    mutate(preparedKey)->write(key);
    for (int i = 0; i < preparedKey->count(); ++i)
        mutate(preparedKey)->byteAt(i) ^= blind;

    auto preparedHashSink = hashSink->copy();
    preparedHashSink->write(preparedKey);
    return preparedHashSink;
}

HashMacSink::HashMacSink(const CryptoHashSink *hashSink, const CharArray *key):
    outerHashSink_{prepareHashSink(hashSink, key, 0x5CU)},
    innerHashSink_{prepareHashSink(hashSink, key, 0x36U)}
{}

HashMacSink::HashMacSink(const HashMacSink *other):
    outerHashSink_{other->outerHashSink_->copy()},
    innerHashSink_{other->innerHashSink_->copy()}
{}

Ref<CryptoHashSink> HashMacSink::copy() const
{
    return new HashMacSink{this};
}

int HashMacSink::blockSize() const
{
    return innerHashSink_->blockSize();
}

void HashMacSink::write(const CharArray *data)
{
    innerHashSink_->write(data);
}

String HashMacSink::finish()
{
    outerHashSink_->write(innerHashSink_->finish());
    return outerHashSink_->finish();
}

}} // cc::crypto
