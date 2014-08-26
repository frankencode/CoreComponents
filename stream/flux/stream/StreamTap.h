/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXSTREAM_STREAMTAP_H
#define FLUXSTREAM_STREAMTAP_H

#include <flux/Stream.h>

namespace flux {
namespace stream {

class StreamTap: public Stream
{
public:
	static Ref<StreamTap> open(Stream *stream, Stream *inputTap, Stream *outputTap);

	inline Stream *stream() const { return stream_; }

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *buf);
	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts);

private:
	StreamTap(Stream *stream, Stream *inputTap, Stream *outputTap);

	Ref<Stream> stream_, inputTap_, outputTap_;
};

}} // namespace flux::stream

#endif // FLUXSTREAM_STREAMTAP_H