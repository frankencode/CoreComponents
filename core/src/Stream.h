/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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
	inline off_t transferAll(Stream *sink, ByteArray *buf = 0) { return transfer(-1, sink, buf); }
	inline off_t skip(off_t count) { return transfer(count); }
	inline void drain() { transfer(); }

	int readAll(ByteArray *data);
	String readAll(int count = -1);
};

} // namespace flux

#endif // FLUX_STREAM_H
