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

CryptoHashSink HashMacSink::Instance::prepareHashSink(const CryptoHashSink &hashSink, const CharArray *key, uint8_t blind)
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

HashMacSink::Instance::Instance(const CryptoHashSink &hashSink, const CharArray *key):
    outerHashSink_{prepareHashSink(hashSink, key, 0x5CU)},
    innerHashSink_{prepareHashSink(hashSink, key, 0x36U)}
{}

HashMacSink::Instance::Instance(const Instance *other):
    outerHashSink_{other->outerHashSink_->copy()},
    innerHashSink_{other->innerHashSink_->copy()}
{}

CryptoHashSink HashMacSink::Instance::copy() const
{
    return HashMacSink{new Instance{this}};
}

int HashMacSink::Instance::blockSize() const
{
    return innerHashSink_->blockSize();
}

void HashMacSink::Instance::write(const CharArray *data)
{
    innerHashSink_->write(data);
}

String HashMacSink::Instance::finish()
{
    outerHashSink_->write(innerHashSink_->finish());
    return outerHashSink_->finish();
}

}} // cc::crypto
