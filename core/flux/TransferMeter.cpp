/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "TransferMeter.h"

namespace flux
{

Ref<TransferMeter> TransferMeter::open(Stream *stream) {
	return new TransferMeter(stream);
}

TransferMeter::TransferMeter(Stream *stream)
	: stream_(stream),
	  totalRead_(0),
	  totalWritten_(0)
{}

bool TransferMeter::readyRead(double interval) const
{
	return stream_->readyRead(interval);
}

int TransferMeter::read(ByteArray *buf)
{
	int n = stream_->read(buf);
	totalRead_ += n;
	return n;
}

void TransferMeter::write(const ByteArray *buf)
{
	stream_->write(buf);
	totalWritten_ += buf->count();
}

void TransferMeter::write(const StringList *parts)
{
	stream_->write(parts);
	for (int i = 0, n = parts->count(); i < n; ++i)
		totalWritten_ += parts->at(i)->count();
}

} // namespace flux
