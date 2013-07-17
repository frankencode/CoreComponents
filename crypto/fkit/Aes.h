/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_AES_H
#define FKIT_AES_H

#include "BlockCipher.h"

namespace fkit
{

class Aes: public BlockCipher
{
public:
	inline static Ref<Aes> create(ByteArray *key) { return new Aes(key); }

	void encode(ByteArray *p, ByteArray *c);
	void decode(ByteArray *c, ByteArray *p);

private:
	Aes(ByteArray *key);
	const int Nk_, Nr_;
	Ref<ByteArray> s_;
	Ref<ByteArray> w_;
};

} // namespace fkit

#endif // FKIT_AES_H
