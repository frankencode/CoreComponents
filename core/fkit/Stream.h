/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_STREAM_H
#define FKIT_STREAM_H

#include "String.h"

namespace fkit
{

FKIT_EXCEPTION(StreamIoException, Exception);
FKIT_EXCEPTION(EncodingException, StreamIoException);

class Stream: public Object
{
public:
	virtual ~Stream() {}

	virtual int readAvail(ByteArray *buf) = 0;
	virtual void write(const ByteArray *buf) = 0;

	virtual void write(const StringList *parts, const char *sep = "");

	void read(ByteArray *buf);

	String readAll();
};

} // namespace fkit

#endif // FKIT_STREAM_H
