/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/System>
#include "TimeoutLimiter"

namespace flux {
namespace stream {

Ref<TimeoutLimiter> TimeoutLimiter::open(Stream *stream, double timeout)
{
	return new TimeoutLimiter(stream, timeout);
}

TimeoutLimiter::TimeoutLimiter(Stream *stream, double timeout)
	: stream_(stream),
	  timeout_(timeout)
{}

bool TimeoutLimiter::readyRead(double interval) const
{
	return stream_->readyRead(interval);
}

int TimeoutLimiter::read(ByteArray *buf)
{
	double interval = timeout_ - System::now();
	if (interval <= 0 || !stream_->readyRead(interval))
		throw TimeoutExceeded();
	return stream_->read(buf);
}

void TimeoutLimiter::write(const ByteArray *buf)
{
	stream_->write(buf);
}

void TimeoutLimiter::write(const StringList *parts)
{
	stream_->write(parts);
}

}} // namespace flux::stream
