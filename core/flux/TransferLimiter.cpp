/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "TransferLimiter.h"

namespace flux
{

Ref<TransferLimiter> TransferLimiter::open(Stream *stream, size_t readLimit, size_t writeLimit) {
	return new TransferLimiter(stream, readLimit, writeLimit);
}

TransferLimiter::TransferLimiter(Stream *stream, size_t readLimit, size_t writeLimit)
	: stream_(stream),
	  readLimit_(readLimit),
	  writeLimit_(writeLimit),
	  totalRead_(0),
	  totalWritten_(0)
{}

bool TransferLimiter::readyRead(double interval) const
{
	return stream_->readyRead(interval);
}

int TransferLimiter::read(ByteArray *buf)
{
	if (readLimit_ > 0 && totalRead_ >= readLimit_)
		throw ReadLimitExceeded();
	int n = stream_->read(buf);
	totalRead_ += n;
	return n;
}

void TransferLimiter::write(const ByteArray *buf)
{
	if (writeLimit_ > 0 && totalWritten_ + buf->size() > writeLimit_)
		throw WriteLimitExceeded();
	write(buf);
	totalWritten_ += buf->size();
}

void TransferLimiter::write(const StringList *parts)
{
	size_t h = 0;
	for (int i = 0, n = parts->size(); i < n; ++i)
		h += parts->at(i)->size();
	if (totalWritten_ + h > writeLimit_) throw WriteLimitExceeded();
	write(parts);
	totalWritten_ += h;
}

} // namespace flux
