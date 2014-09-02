/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXCRYPTO_HASHSUM_H
#define FLUXCRYPTO_HASHSUM_H

#include <flux/ByteArray.h>

namespace flux {
namespace crypto {

class HashSum: public Object
{
public:
	virtual void feed(const ByteArray *data) = 0;
	virtual Ref<ByteArray> finish() = 0;
};

}} // namespace flux::crypto

#endif // FLUXCRYPTO_HASHSUM_H
