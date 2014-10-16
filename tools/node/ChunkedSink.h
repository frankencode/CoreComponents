/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_CHUNKEDSINK_H
#define FLUXNODE_CHUNKEDSINK_H

#include <flux/Stream>

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
