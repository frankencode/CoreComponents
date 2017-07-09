/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/HashSink>

namespace cc {
namespace crypto {

/** \class Sha1Sink Sha1Sink.h cc/crypto/Sha1Sink
  * \brief Secure Hash 1: a one-way hash function
  */
class Sha1Sink: public HashSink
{
public:
    enum { Size = 20 };

    static Ref<Sha1Sink> open();

    virtual void write(const ByteArray *data);
    virtual Ref<ByteArray> finish();

private:
    Sha1Sink();
    void consume();

    Ref<ByteArray> h_;
    Ref<ByteArray> m_;
    Ref<ByteArray> w_;
    int j_;
    uint64_t l_;
};

Ref<ByteArray> sha1(const ByteArray *data);

}} // namespace cc::crypto
