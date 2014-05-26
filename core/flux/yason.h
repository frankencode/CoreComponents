/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_YASON_H
#define FLUX_YASON_H

#include "exceptions.h"
#include "YasonObject.h"
#include "YasonProtocol.h"

namespace flux
{

namespace yason
{

Variant parse(const ByteArray *text, const YasonProtocol *protocol = 0);
String stringify(Variant value);

} // namespace yason

} // namespace flux

#endif // FLUX_YASON_H
