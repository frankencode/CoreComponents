/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_NULLSTREAM_H
#define FLUX_NULLSTREAM_H

#include "Stream.h"

namespace flux
{

template<class SubClass> class Singleton;

class NullStream: public Stream
{
public:
	virtual int read(ByteArray *buf);
	virtual void write(const ByteArray *buf);

private:
	friend class Singleton<NullStream>;
	NullStream();
};

Stream *nullStream();

} // namespace flux

#endif // FLUX_NULLSTREAM_H
