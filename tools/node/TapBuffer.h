/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_TAPBUFFER_H
#define FLUXNODE_TAPBUFFER_H

#include <flux/stream/LineBuffer>

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
