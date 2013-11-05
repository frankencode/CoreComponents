/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_TIMEOUTLIMITER_H
#define FLUX_TIMEOUTLIMITER_H

#include "Stream.h"

namespace flux
{

class TimeoutExceeded {};

class TimeoutLimiter: public Stream
{
public:
	static Ref<TimeoutLimiter> open(Stream *stream, double timeout = 0);

	inline Stream *stream() const { return stream_; }
	inline double timeout() const { return timeout_; }

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *buf);
	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts);

private:
	TimeoutLimiter(Stream *stream, double timeout);

	Ref<Stream> stream_;
	double timeout_;
};

} // namespace flux

#endif // FLUX_TIMEOUTLIMITER_H
