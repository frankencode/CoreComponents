/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "System.h"
#include "TimeoutLimiter.h"

namespace flux
{

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

} // namespace flux
