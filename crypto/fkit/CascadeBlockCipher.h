/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_CASCADEBLOCKCIPHER_H
#define FKIT_CASCADEBLOCKCIPHER_H

#include "BlockCipher.h"

namespace fkit
{

class CascadeBlockCipher: public BlockCipher
{
public:
	CascadeBlockCipher(BlockCipher *cipher, ByteArray *iv);

	void encode(ByteArray *c, ByteArray *p);
	void decode(ByteArray *p, ByteArray *c);

private:
	Ref<BlockCipher> cipher_;
	Ref<ByteArray> s_;
};

} // namespace fkit

#endif // FKIT_CASCADEBLOCKCIPHER_H
