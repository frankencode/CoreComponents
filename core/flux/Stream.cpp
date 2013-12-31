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

int Stream::read(ByteArray *data)
{
	return 0;
}

void Stream::write(const ByteArray *data)
{}

void Stream::write(const StringList *parts)
{
	for (int i = 0, n = parts->size(); i < n; ++i)
		write(parts->at(i));
}

off_t Stream::transfer(off_t count, Stream *sink, ByteArray *buf)
{
	if (count == 0) return 0;

	off_t total = 0;
	Ref<ByteArray> h;
	if (!buf) {
		h = ByteArray::allocate((0 < count && count < 0x4000) ? count : 0x4000);
		buf = h;
	}

	while (true) {
		int n = read(buf);
		if (n == 0) break;
		if (sink) sink->write(ByteRange(buf, 0, n));
		total += n;
		if (count > 0) {
			count -= n;
			if (count == 0) break;
			if (count < buf->size()) buf->truncate(count);
		}
	}

	return total;
}

int Stream::readAll(ByteArray *data)
{
	const int w = data->size();
	int m = 0;
	while (m < w) {
		int n = read(ByteRange(data, m, w));
		if (n == 0) break;
		m += n;
	}
	return m;
}

String Stream::readAll(int count)
{
	if (count == 0) return String();

	if (count < 0) {
		Ref<StringList> parts = StringList::create();
		Ref<ByteArray> data = ByteArray::allocate(0x4000);
		while (true) {
			int n = read(data);
			if (n == 0) break;
			parts->append(data->copy(0, n));
		}
		return parts->join();
	}

	String s(count);
	int n = readAll(s);
	if (n == 0) return String();
	s->truncate(n);
	return s;
}

} // namespace flux
