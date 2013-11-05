/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_BYTECIPHER_H
#define FLUX_BYTECIPHER_H

#include <stdint.h>
#include <flux/Object.h>

namespace flux
{

class ByteCipher: public Object
{
public:
	virtual uint8_t encode(uint8_t p) = 0;
	virtual uint8_t decode(uint8_t c) = 0;
};

} // namespace flux

#endif // FLUX_BYTECIPHER_H
