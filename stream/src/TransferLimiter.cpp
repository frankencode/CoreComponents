/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stream/TransferLimiter>

namespace flux {
namespace stream {

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
	if (writeLimit_ > 0 && totalWritten_ + buf->count() > writeLimit_)
		throw WriteLimitExceeded();
	write(buf);
	totalWritten_ += buf->count();
}

void TransferLimiter::write(const StringList *parts)
{
	size_t h = 0;
	for (int i = 0, n = parts->count(); i < n; ++i)
		h += parts->at(i)->count();
	if (totalWritten_ + h > writeLimit_) throw WriteLimitExceeded();
	write(parts);
	totalWritten_ += h;
}

}} // namespace flux::stream
