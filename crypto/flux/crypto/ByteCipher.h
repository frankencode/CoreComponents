/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXCRYPTO_BYTECIPHER_H
#define FLUXCRYPTO_BYTECIPHER_H

#include <stdint.h>
#include <flux/Object.h>

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
