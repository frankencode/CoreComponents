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

namespace flux
{

bool Stream::readyRead(double interval) const
{
	return true;
}

int Stream::read(ByteArray *buf)
{
	return 0;
}

off_t Stream::skip(off_t count)
{
	return 0;
}

off_t Stream::transfer(Stream *sink, off_t count, ByteArray *buf)
{
	if (!sink) return skip(count);

	off_t total = 0;
	Ref<ByteArray> h;
	if (!buf) {
		h = ByteArray::create(0x4000 > count ? count : 0x4000);
		buf = h;
	}

	while (true) {
		int n = read(buf);
		if (n == 0) break;
		sink->write(ByteRange(buf, 0, n));
		total += n;
		if (count > 0) {
			count -= n;
			if (count == 0) break;
			if (count < buf->size()) buf->truncate(count);
		}
	}

	return total;
}

void Stream::write(const ByteArray *buf)
{}

void Stream::write(const StringList *parts)
{
	for (int i = 0, n = parts->size(); i < n; ++i)
		write(parts->at(i));
}

String Stream::readAll()
{
	Ref<StringList> parts = StringList::create();
	String s(0x3FFF);
	while (true) {
		int n = read(s);
		if (n == 0) break;
		parts->append(s->copy(0, n));
	}
	return parts->join();
}

void Stream::drain()
{
	String s(0x3FFF);
	while (true) {
		int n = read(s);
		if (n == 0) break;
	}
}

} // namespace flux
