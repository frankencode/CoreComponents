/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/Format.h>
#include <fkit/stdio.h> // DEBUG
#include "PayloadSink.h"

namespace fnode
{

Ref<PayloadSink> PayloadSink::open(Stream *stream)
{
	return new PayloadSink(stream);
}

PayloadSink::PayloadSink(Stream *stream)
	: stream_(stream)
{}

PayloadSink::~PayloadSink()
{
	Format chunk(stream_);
	chunk << 0 << "\r\n" << "\r\n";
	ferr() << chunk->join();
}

void PayloadSink::write(const ByteArray *buf)
{
	Format chunk(stream_);
	chunk << hex(buf->size()) << "\r\n" << buf << "\r\n";
	ferr() << chunk->join();
}

void PayloadSink::write(const StringList *parts, const char *sep)
{
	Format chunk(stream_);
	int total = 0;
	for (int i = 0; i < parts->size(); ++i)
		total += parts->at(i)->size();
	chunk << hex(total) << "\r\n";
	for (int i = 0; i < parts->size(); ++i)
		chunk << parts->at(i);
	chunk << "\r\n";
	ferr() << chunk->join();
}

} // namespace fnode
