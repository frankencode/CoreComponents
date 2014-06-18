/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "CascadeBlockCipher.h"

namespace flux {
namespace crypto {

CascadeBlockCipher::CascadeBlockCipher(BlockCipher *cipher, ByteArray *iv)
	: BlockCipher(cipher->blockSize()),
	  cipher_(cipher),
	  s_(iv->copy(0, cipher->blockSize()))
{}

void CascadeBlockCipher::encode(ByteArray *p, ByteArray *c)
{
	*s_ ^= *p;
	cipher_->encode(s_, c);
	*s_ = *c;
}

void CascadeBlockCipher::decode(ByteArray *c, ByteArray *p)
{
	cipher_->decode(c, p);
	*p ^= *s_;
	*s_ = *c;
}

}} // namespace flux::crypto
