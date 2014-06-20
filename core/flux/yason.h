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
#include "MetaObject.h"
#include "MetaProtocol.h"

namespace flux {
namespace yason {

Variant parse(const ByteArray *text, const MetaProtocol *protocol = 0);
String stringify(Variant value);

}} // namespace flux::yason

#endif // FLUX_YASON_H
