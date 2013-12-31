/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_HASHMETER_H
#define FLUX_HASHMETER_H

#include <flux/Stream.h>
#include "HashSum.h"

namespace flux
{

class HashMeter: public Stream
{
public:
	static Ref<HashMeter> open(HashSum *hashSum, Stream *stream = 0);

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *data);

	virtual void write(const ByteArray *data);
	virtual void write(const StringList *parts);

	Ref<ByteArray> finish();

private:
	HashMeter(HashSum *hashSum, Stream *stream);

	Ref<HashSum> hashSum_;
	Ref<Stream> stream_;
};

} // namespace flux

#endif // FLUX_HASHMETER_H
