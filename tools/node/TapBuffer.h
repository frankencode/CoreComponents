/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_TAPBUFFER_H
#define FLUXNODE_TAPBUFFER_H

#include <flux/stream/LineBuffer.h>

namespace fluxnode
{

using namespace flux;
using namespace flux::stream;

class TapBuffer: public LineBuffer
{
public:
	static Ref<TapBuffer> open(Stream *stream, String prefix);

	virtual String prefix() const;

	void write(const ByteArray *buf);

private:
	TapBuffer(Stream *stream, String prefix);

	String prefix_;
};

} // namespace fluxnode

#endif // FLUXNODE_TAPBUFFER_H
