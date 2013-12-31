/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_MD5_H
#define FLUX_MD5_H

#include "HashSum.h"

namespace flux
{

class Md5: public HashSum
{
public:
	enum { Size = 16 };

	static Ref<Md5> create();

	virtual void feed(const ByteArray *data);
	virtual Ref<ByteArray> finish();

private:
	Md5();
	void consume();

	Ref<ByteArray> aux_;
	int auxFill_;
	uint64_t bytesFeed_;
	uint32_t a_, b_, c_, d_;
};

Ref<ByteArray> md5(ByteArray *data);

} // namespace flux

#endif // FLUX_MD5_H
