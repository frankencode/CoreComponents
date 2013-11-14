/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ZStream.h"

namespace flux
{

ZStream::ZStream()
{
	next_in = 0;
	avail_in = 0;
	total_in = 0;

	next_out = 0;
	avail_out = 0;
	total_out = 0;

	zalloc = &ZStream::alloc;
	zfree = &ZStream::free;
	opaque = 0;
}

void *ZStream::alloc(void *opaque, uInt items, uInt size)
{
	return new char[size * items];
}

void ZStream::free(void *opaque, void *address)
{
	delete[] ((char *)address);
}

} // namespace flux
