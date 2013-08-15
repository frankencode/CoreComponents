/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "types.h"
#include "Stream.h"

namespace fkit
{

void Stream::read(ByteArray *buf)
{
	int bufFill = 0;
	while (bufFill < buf->size()) {
		int n = readAvail(ByteRange(buf, bufFill, buf->size()));
		if (n == 0)
			FKIT_THROW(StreamIoException, "Reading beyond end of input");
		bufFill += n;
	}
}

void Stream::write(const StringList *parts, const char *sep)
{
	write(parts->join(sep));
}

String Stream::readAll()
{
	Ref<StringList> parts = StringList::create();
	String s(0x4000);
	while (true) {
		int n = readAvail(s);
		if (n == 0) break;
		parts->append(s->copy(0, n));
	}
	return parts->join();
}

} // namespace fkit