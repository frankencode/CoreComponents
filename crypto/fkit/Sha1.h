/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_SHA1_H
#define FKIT_SHA1_H

#include <fkit/ByteArray.h>

namespace fkit
{

class Sha1: public Object
{
public:
	enum { size = 20 };

	inline static Ref<Sha1> create() { return new Sha1; }

	void feed(ByteArray *chunk);
	void finish();

	ByteArray *sum() const;

private:
	Sha1();
	void consume();

	Ref<ByteArray> h_;
	Ref<ByteArray> m_;
	Ref<ByteArray> w_;
	int j_;
	uint64_t l_;
};

Ref<ByteArray> sha1(ByteArray *message);

} // namespace fkit

#endif // FKIT_SHA1_H
