 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include "types.hpp"
#include "Buffer.hpp"
#include "Stream.hpp"

namespace ftl
{

void Stream::read(void *buf, int bufFill)
{
	uint8_t *p = (uint8_t *)buf;
	int n = bufFill;
	while (n > 0) {
		int dn = readAvail(p, n);
		if (dn == 0)
			FTL_THROW(StreamIoException, "Reading beyond end of input");
		p += dn;
		n -= dn;
	}
}

String Stream::readAll()
{
	O<Buffer> buffer = new Buffer;
	while (true) {
		Block *block = buffer->allocate();
		block->setFill(readAvail(block->data(), buffer->blockSize()));
		if (block->fill() == 0) break;
	}
	return buffer->join();
}

} // namespace ftl
