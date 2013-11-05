/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_TAPBUFFER_H
#define FLUX_TAPBUFFER_H

#include <flux/LineBuffer.h>

namespace fnode
{

using namespace flux;

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

} // namespace fnode

#endif // FLUX_TAPBUFFER_H
