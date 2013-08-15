/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "NullStream.h"

namespace fkit
{

NullStream::NullStream()
{}

int NullStream::readAvail(ByteArray *buf)
{
	buf->clear();
	return buf->size();
}

void NullStream::write(const ByteArray *buf)
{}

} // namespace fkit
