/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXSTREAM_TIMEOUTLIMITER_H
#define FLUXSTREAM_TIMEOUTLIMITER_H

#include <flux/Stream.h>
#include <flux/Exception.h>

namespace flux {
namespace stream {

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

class TimeoutExceeded: public Exception
{
public:
	~TimeoutExceeded() throw() {}

	virtual String message() const { return "Timeout exceeded"; }
};

}} // namespace flux::stream

#endif // FLUXSTREAM_TIMEOUTLIMITER_H
