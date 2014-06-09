/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Format.h>
#include "ChunkedSink.h"

namespace fluxnode
{

Ref<ChunkedSink> ChunkedSink::open(Stream *stream)
{
	return new ChunkedSink(stream);
}

ChunkedSink::ChunkedSink(Stream *stream)
	: stream_(stream)
{}

ChunkedSink::~ChunkedSink()
{
	Format(stream_) << 0 << "\r\n" << "\r\n";
}

void ChunkedSink::write(const ByteArray *buf)
{
	Format(stream_) << hex(buf->count()) << "\r\n" << buf << "\r\n";
}

void ChunkedSink::write(const StringList *parts)
{
	Format chunk(stream_);
	int total = 0;
	for (int i = 0; i < parts->count(); ++i)
		total += parts->at(i)->count();
	chunk << hex(total) << "\r\n";
	for (int i = 0; i < parts->count(); ++i)
		chunk << parts->at(i);
	chunk << "\r\n";
}

} // namespace fluxnode
