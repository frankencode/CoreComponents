/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_HASHSUM_H
#define FLUX_HASHSUM_H

#include <flux/ByteArray.h>

namespace flux
{

class HashSum: public Object
{
public:
	virtual void feed(const ByteArray *data) = 0;
	virtual Ref<ByteArray> finish() = 0;
};

} // namespace flux

#endif // FLUX_HASHSUM_H
