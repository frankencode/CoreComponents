/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/crypto/MessageAuthenticationSink>

namespace cc {
namespace crypto {

Ref<MessageAuthenticationSink> MessageAuthenticationSink::open(const CharArray *key, const CryptoHashSink *hashSink)
{
    return new MessageAuthenticationSink{key, hashSink};
}

Ref<CryptoHashSink> MessageAuthenticationSink::prepareHashSink(const CryptoHashSink *hashSink, const CharArray *key, uint8_t blind)
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

MessageAuthenticationSink::MessageAuthenticationSink(const CharArray *key, const CryptoHashSink *hashSink):
    outerHashSink_{prepareHashSink(hashSink, key, 0x36U)},
    innerHashSink_{prepareHashSink(hashSink, key, 0x5CU)}
{}

MessageAuthenticationSink::MessageAuthenticationSink(const MessageAuthenticationSink *other):
    outerHashSink_{other->outerHashSink_->copy()},
    innerHashSink_{other->innerHashSink_->copy()}
{}

Ref<CryptoHashSink> MessageAuthenticationSink::copy() const
{
    return new MessageAuthenticationSink{this};
}

int MessageAuthenticationSink::blockSize() const
{
    return innerHashSink_->blockSize();
}

void MessageAuthenticationSink::write(const CharArray *data)
{
    innerHashSink_->write(data);
}

String MessageAuthenticationSink::finish()
{
    outerHashSink_->write(innerHashSink_->finish());
    return outerHashSink_->finish();
}

}} // cc::crypto
