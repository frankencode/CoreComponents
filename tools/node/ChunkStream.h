/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_CHUNKSTREAM_H
#define FNODE_CHUNKSTREAM_H

#include <fkit/Stream.h>

namespace fnode
{

using namespace fkit;

class ChunkStream: public Stream
{
public:
	static Ref<ChunkStream> open(Stream *client = 0);

	virtual int readAvail(ByteArray *buf);
	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts, const char *sep = "");

private:
	ChunkStream(Stream *client);
	~ChunkStream();
	Ref<Stream> client_;
};

} // namespace fnode

#endif // FNODE_CHUNKSTREAM_H
