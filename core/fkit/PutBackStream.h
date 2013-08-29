/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_PUTBACKSTREAM_H
#define FKIT_PUTBACKSTREAM_H

#include "Stream.h"

namespace fkit
{

class Mutex;

class PutBackStream: public Stream
{
public:
	static Ref<PutBackStream> open(Stream *stream);

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *buf);
	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts);

	void putBack(ByteArray *pending);

private:
	PutBackStream(Stream *stream);

	Ref<Stream> stream_;
	Ref<Mutex> mutex_;
	Ref<ByteArray> pending_;
};

} // namespace fkit

#endif // FKIT_PUTBACKSTREAM_H
