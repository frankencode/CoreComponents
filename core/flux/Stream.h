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

class Stream: public Object
{
public:
	virtual ~Stream() {}

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *data);

	virtual void write(const ByteArray *data);
	virtual void write(const StringList *parts);

	virtual off_t transfer(off_t count = -1, Stream *sink = 0, ByteArray *buf = 0);
	inline off_t skip(off_t count) { return transfer(count); }
	inline void drain() { transfer(); }

	int readAll(ByteArray *data);
	String readAll(int count = -1);
};

} // namespace flux

#endif // FLUX_STREAM_H
