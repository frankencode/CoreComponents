/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_NULLSTREAM_H
#define FKIT_NULLSTREAM_H

#include "Singleton.h"
#include "Stream.h"

namespace fkit
{

class NullStream: public Stream, public Singleton<NullStream>
{
public:
	virtual int read(ByteArray *buf);
	virtual void write(const ByteArray *buf);

private:
	NullStream();
	friend class Singleton<NullStream>;
};

inline Stream *nullStream() { return NullStream::instance(); }

} // namespace fkit

#endif // FKIT_NULLSTREAM_H
