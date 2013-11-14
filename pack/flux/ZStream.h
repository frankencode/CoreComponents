/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_ZSTREAM_H
#define FLUX_ZSTREAM_H

#include <zlib.h>
#include <flux/Object.h>

namespace flux
{

class ZStream: public z_stream, public Object
{
public:
	static Ref<ZStream> create() { return new ZStream; }

private:
	ZStream();
	static void *alloc(void *opaque, uInt items, uInt size);
	static void free(void *opaque, void *address);
};

} // namespace flux

#endif // FLUX_ZSTREAM_H
