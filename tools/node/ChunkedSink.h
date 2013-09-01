/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_CHUNKEDSINK_H
#define FNODE_CHUNKEDSINK_H

#include <fkit/Stream.h>

namespace fnode
{

using namespace fkit;

class ChunkedSink: public Stream
{
public:
	static Ref<ChunkedSink> open(Stream *stream);

	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts);

private:
	ChunkedSink(Stream *client);
	~ChunkedSink();
	Ref<Stream> stream_;
};

} // namespace fnode

#endif // FNODE_CHUNKEDSINK_H
