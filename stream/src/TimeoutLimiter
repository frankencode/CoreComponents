/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXSTREAM_TIMEOUTLIMITER_H
#define FLUXSTREAM_TIMEOUTLIMITER_H

#include <flux/Stream>
#include <flux/Exception>

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
