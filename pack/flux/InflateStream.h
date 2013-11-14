/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_INFLATESTREAM_H
#define FLUX_INFLATESTREAM_H

#include <flux/Stream.h>

namespace flux
{

class ZStream;

class InflateError {
public:
	InflateError(int error): error_(error) {}
	inline int error() const { return error_; }

private:
	int error_;
};

class InflateStream: public Stream
{
public:
	static Ref<InflateStream> open(Stream *source, ByteArray *buf = 0);

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *buf);

private:
	InflateStream(Stream *source, ByteArray *buf);
	~InflateStream();

	Ref<Stream> source_;
	Ref<ByteArray> zBuf_;
	Ref<ZStream> z_;
};

} // namespace flux

#endif // FLUX_INFLATESTREAM_H
