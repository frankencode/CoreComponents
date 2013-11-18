/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_STREAM_H
#define FLUX_STREAM_H

#include "String.h"

namespace flux
{

FLUX_EXCEPTION(EncodingException, Exception); // FIXME

class Stream: public Object
{
public:
	virtual ~Stream() {}

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *buf);
	virtual off_t skip(off_t n);
	virtual off_t transfer(Stream *sink, off_t count = -1, ByteArray *buf = 0);

	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts);

	String readAll();
	void drain();
};

} // namespace flux

#endif // FLUX_STREAM_H
