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

/** \class Sha1 Sha1.h cc/crypto/Sha1
  * \brief Secure Hash 1: a one-way hash function
  */
class Sha1: public HashSum
{
public:
    enum { Size = 20 };

    static Ref<Sha1> create();

    virtual void write(const ByteArray *data);
    virtual Ref<ByteArray> finish();

private:
    Sha1();
    void consume();

    Ref<ByteArray> h_;
    Ref<ByteArray> m_;
    Ref<ByteArray> w_;
    int j_;
    uint64_t l_;
};

Ref<ByteArray> sha1(const ByteArray *data);

}} // namespace cc::crypto
