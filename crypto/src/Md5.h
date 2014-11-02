/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXCRYPTO_MD5_H
#define FLUXCRYPTO_MD5_H

#include <flux/crypto/HashSum>

namespace flux {
namespace crypto {

/** \brief Message Digest 5: a one-way hash function
  */
class Md5: public HashSum
{
public:
    enum { Size = 16 };

    static Ref<Md5> create();

    virtual void feed(const ByteArray *data);
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

}} // namespace flux::crypto

#endif // FLUXCRYPTO_MD5_H
