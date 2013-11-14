/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_DEFLATESTREAM_H
#define FLUX_DEFLATESTREAM_H

#include <flux/Stream.h>

namespace flux
{

class ZStream;

class DeflateStream: public Stream
{
public:
	static Ref<DeflateStream> open(Stream *sink, ByteArray *buf = 0, bool gzip = false, int level = -1, int strategy = -1);

	virtual void write(const ByteArray *buf);

private:
	DeflateStream(Stream *sink, ByteArray *buf, bool zip, int level, int strategy);
	~DeflateStream();

	Ref<Stream> sink_;
	Ref<ByteArray> zBuf_;
	Ref<ZStream> z_;
};

} // namespace flux

#endif // FLUX_DEFLATESTREAM_H
