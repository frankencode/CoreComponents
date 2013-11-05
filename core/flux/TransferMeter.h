/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_TRANSFERMETER_H
#define FLUX_TRANSFERMETER_H

#include "Stream.h"

namespace flux
{

class TransferMeter: public Stream
{
public:
	static Ref<TransferMeter> open(Stream *stream);

	inline Stream *stream() const { return stream_; }
	inline size_t totalRead() const { return totalRead_; }
	inline size_t totalWritten() const { return totalWritten_; }

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *buf);
	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts);

private:
	TransferMeter(Stream *stream);

	Ref<Stream> stream_;
	size_t totalRead_;
	size_t totalWritten_;
};

} // namespace flux

#endif // FLUX_TRANSFERMETER_H
