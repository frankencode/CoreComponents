/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_LOOKAHEADSTREAM_H
#define FLUX_LOOKAHEADSTREAM_H

#include "Stream.h"

namespace flux
{

class LookAheadStream: public Stream
{
public:
	static Ref<LookAheadStream> open(Stream *source, int windowSize);

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *buf);
	virtual off_t transfer(off_t count = -1, Stream *sink = 0, ByteArray *buf = 0);

	void replay();
	void done();

private:
	LookAheadStream(Stream *source, int windowSize);

	Ref<Stream> source_;
	Ref<ByteArray> window_;
	const int w_; // window size
	int m_; // window fill
	int i_; // read offset
	bool done_;
};

} // namespace flux

#endif // FLUX_LOOKAHEADSTREAM_H
