/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXSTREAM_TRANSFERLIMITER_H
#define FLUXSTREAM_TRANSFERLIMITER_H

#include <flux/Stream.h>
#include <flux/Exception.h>

namespace flux {
namespace stream {

class TransferLimiter: public Stream
{
public:
	static Ref<TransferLimiter> open(Stream *stream, size_t readLimit, size_t writeLimit = 0);

	inline Stream *stream() const { return stream_; }
	inline size_t readLimit() const { return readLimit_; }
	inline size_t writeLimit() const { return writeLimit_; }
	inline size_t totalRead() const { return totalRead_; }
	inline size_t totalWritten() const { return totalWritten_; }

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *buf);
	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts);

private:
	TransferLimiter(Stream *stream, size_t readLimit, size_t writeLimit);

	Ref<Stream> stream_;
	size_t readLimit_;
	size_t writeLimit_;
	size_t totalRead_;
	size_t totalWritten_;
};

class TransferLimitExceeded: public Exception
{
public:
	~TransferLimitExceeded() throw() {}
};

class ReadLimitExceeded: public TransferLimitExceeded
{
public:
	~ReadLimitExceeded() throw() {}

	virtual String message() const { return "Read transfer limit exceeded"; }
};

class WriteLimitExceeded: public TransferLimitExceeded
{
public:
	~WriteLimitExceeded() throw() {}

	virtual String message() const { return "Write transfer limit exceeded"; }
};

}} // namespace flux::stream

#endif // FLUXSTREAM_TRANSFERLIMITER_H
