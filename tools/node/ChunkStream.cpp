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
#include "ChunkStream.h"

namespace fnode
{

Ref<ChunkStream> ChunkStream::open(Stream *client)
{
	return new ChunkStream(client);
}

ChunkStream::ChunkStream(Stream *client)
	: client_(client)
{}

ChunkStream::~ChunkStream()
{
	if (!client_) return;

	Format chunk(client_);
	chunk << 0 << "\r\n" << "\r\n";
	ferr() << chunk->join();
}

int ChunkStream::readAvail(ByteArray *buf) { return 0; }

void ChunkStream::write(const ByteArray *buf)
{
	if (!client_) return;

	Format chunk(client_);
	chunk << hex(buf->size()) << "\r\n" << buf << "\r\n";
	ferr() << chunk->join();
}

void ChunkStream::write(const StringList *parts, const char *sep)
{
	if (!client_) return;

	Format chunk(client_);
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
