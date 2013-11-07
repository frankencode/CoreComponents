/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_CHUNKEDSINK_H
#define FLUXNODE_CHUNKEDSINK_H

#include <flux/Stream.h>

namespace fluxnode
{

using namespace flux;

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

} // namespace fluxnode

#endif // FLUXNODE_CHUNKEDSINK_H
