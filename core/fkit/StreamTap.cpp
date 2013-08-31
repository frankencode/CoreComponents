/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "StreamTap.h"

namespace fkit
{

Ref<StreamTap> StreamTap::open(Stream *stream, Stream *inputTap, Stream *outputTap)
{
	return new StreamTap(stream, inputTap, outputTap);
}

StreamTap::StreamTap(Stream *stream, Stream *inputTap, Stream *outputTap)
	: stream_(stream),
	  inputTap_(inputTap),
	  outputTap_(outputTap)
{}

bool StreamTap::readyRead(double interval) const
{
	return stream_->readyRead(interval);
}

int StreamTap::read(ByteArray *buf)
{
	int n = stream_->read(buf);
	inputTap_->write(ByteRange(buf, 0, n));
	return n;
}

void StreamTap::write(const ByteArray *buf)
{
	outputTap_->write(buf);
	stream_->write(buf);
}

void StreamTap::write(const StringList *parts)
{
	outputTap_->write(parts);
	stream_->write(parts);
}

} // namespace fkit
