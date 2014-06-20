/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Format.h"
#include "YasonSyntax.h"
#include "YasonWriter.h"
#include "yason.h"

namespace flux {
namespace yason {

Variant parse(const ByteArray *text, const MetaProtocol *protocol)
{
	return yasonSyntax()->parse(text, protocol);
}

String stringify(Variant value)
{
	Format format;
	YasonWriter::create(format)->write(value);
	return format;
}

}} // namespace flux::yason
