/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXCRYPTO_BYTECIPHER_H
#define FLUXCRYPTO_BYTECIPHER_H

#include <stdint.h>
#include <flux/Object>

namespace flux {
namespace crypto {

class ByteCipher: public Object
{
public:
	virtual uint8_t encode(uint8_t p) = 0;
	virtual uint8_t decode(uint8_t c) = 0;
};

}} // namespace flux::crypto

#endif // FLUXCRYPTO_BYTECIPHER_H
