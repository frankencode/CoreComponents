/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXCRYPTO_SHA1_H
#define FLUXCRYPTO_SHA1_H

#include <flux/crypto/HashSum>

namespace flux {
namespace crypto {

/** \brief Secure Hash 1: a one-way hash function
  */
class Sha1: public HashSum
{
public:
    enum { Size = 20 };

    static Ref<Sha1> create();

    virtual void feed(const ByteArray *data);
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

}} // namespace flux::crypto

#endif // FLUXCRYPTO_SHA1_H
