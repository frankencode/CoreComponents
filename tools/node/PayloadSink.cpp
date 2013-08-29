/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/Format.h>
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
	Format(stream_) << 0 << "\r\n" << "\r\n";
}

void PayloadSink::write(const ByteArray *buf)
{
	Format(stream_) << hex(buf->size()) << "\r\n" << buf << "\r\n";
}

void PayloadSink::write(const StringList *parts)
{
	Format chunk(stream_);
	int total = 0;
	for (int i = 0; i < parts->size(); ++i)
		total += parts->at(i)->size();
	chunk << hex(total) << "\r\n";
	for (int i = 0; i < parts->size(); ++i)
		chunk << parts->at(i);
	chunk << "\r\n";
}

} // namespace fnode
