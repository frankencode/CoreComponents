/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_TRANSFERLIMITER_H
#define FKIT_TRANSFERLIMITER_H

#include "Stream.h"

namespace fkit
{

class TransferLimitExceeded {};
class ReadLimitExceeded: public TransferLimitExceeded {};
class WriteLimitExceeded: public TransferLimitExceeded {};

class TransferLimiter: public Stream
{
public:
	static Ref<TransferLimiter> open(Stream *stream, size_t readLimit, size_t writeLimit = 0);

	virtual int readAvail(ByteArray *buf);
	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts, const char *sep = "");

	inline size_t readLimit() const { return readLimit_; }
	inline size_t writeLimit() const { return writeLimit_; }
	inline size_t totalRead() const { return totalRead_; }
	inline size_t totalWritten() const { return totalWritten_; }

	inline Stream *stream() const { return stream_; }

private:
	TransferLimiter(Stream *stream, size_t readLimit, size_t writeLimit);

	Ref<Stream> stream_;
	size_t readLimit_;
	size_t writeLimit_;
	size_t totalRead_;
	size_t totalWritten_;
};

} // namespace fkit

#endif // FKIT_TRANSFERLIMITER_H
