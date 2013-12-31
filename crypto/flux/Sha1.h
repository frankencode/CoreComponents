/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_SHA1_H
#define FLUX_SHA1_H

#include "HashSum.h"

namespace flux
{

class Sha1: public HashSum
{
public:
	enum { Size = 20 };

	static Ref<Sha1> create();

	virtual void feed(const ByteArray *data);
	virtual Ref<ByteArray> finish();

private:
	Sha1();
	void consume();

	Ref<ByteArray> h_;
	Ref<ByteArray> m_;
	Ref<ByteArray> w_;
	int j_;
	uint64_t l_;
};

Ref<ByteArray> sha1(ByteArray *data);

} // namespace flux

#endif // FLUX_SHA1_H
