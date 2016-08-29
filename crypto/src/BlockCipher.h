/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/ByteArray>

namespace cc {
namespace crypto {

/** \brief Cryptographic block cipher
  */
class BlockCipher: public Object
{
public:
    inline int blockSize() const { return blockSize_; }

    virtual void encode(const ByteArray *c, ByteArray *p) = 0;
    virtual void decode(const ByteArray *c, ByteArray *p) = 0;

protected:
    BlockCipher(int blockSize): blockSize_(blockSize) {}

private:
    int blockSize_;
};

}} // namespace cc::crypto
