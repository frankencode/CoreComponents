/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/HashSum>

namespace cc {
namespace crypto {

/** \class Md5 Md5.h cc/crypto/Md5
  * \brief Message Digest 5: a one-way hash function
  */
class Md5: public HashSum
{
public:
    enum { Size = 16 };

    static Ref<Md5> create();

    virtual void write(const ByteArray *data);
    virtual Ref<ByteArray> finish();

private:
    Md5();
    void consume();

    Ref<ByteArray> aux_;
    int auxFill_;
    uint64_t bytesFeed_;
    uint32_t a_, b_, c_, d_;
};

Ref<ByteArray> md5(const ByteArray *data);

}} // namespace cc::crypto
