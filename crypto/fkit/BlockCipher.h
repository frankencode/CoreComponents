/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_BLOCKCIPHER_H
#define FKIT_BLOCKCIPHER_H

#include <fkit/ByteArray.h>

namespace fkit
{

class BlockCipher: public Object
{
public:
	inline int blockSize() const { return blockSize_; }

	virtual void encode(ByteArray *c, ByteArray *p) = 0;
	virtual void decode(ByteArray *c, ByteArray *p) = 0;

protected:
	BlockCipher(int blockSize): blockSize_(blockSize) {}

private:
	int blockSize_;
};

} // namespace fkit

#endif // FKIT_BLOCKCIPHER_H
