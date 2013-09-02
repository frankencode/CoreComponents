/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/Thread.h>
#include <fkit/str.h>
#include "TapBuffer.h"

namespace fnode
{

Ref<TapBuffer> TapBuffer::open(Stream *stream, String prefix)
{
	return new TapBuffer(stream, prefix);
}

TapBuffer::TapBuffer(Stream *stream, String prefix)
	: LineBuffer(stream),
	  prefix_(prefix)
{}

String TapBuffer::prefix() const
{
	return "(" + inum(thread()->id(), 62) + ") " + prefix_;
}

} // namespace fnode
