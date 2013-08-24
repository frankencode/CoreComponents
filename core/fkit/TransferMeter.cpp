/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "TransferMeter.h"

namespace fkit
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

int TransferMeter::readAvail(ByteArray *buf)
{
	int n = stream_->readAvail(buf);
	totalRead_ += n;
	return n;
}

void TransferMeter::write(const ByteArray *buf)
{
	write(buf);
	totalWritten_ += buf->size();
}

void TransferMeter::write(const StringList *parts, const char *sep)
{
	write(parts, sep);
	for (int i = 0, n = parts->size(); i < n; ++i)
		totalWritten_ += parts->at(i)->size();
	totalWritten_ += (parts->size() - 1) * strlen(sep);
}

} // namespace fkit
