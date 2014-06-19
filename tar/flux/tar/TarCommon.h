/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTAR_TARCOMMON_H
#define FLUXTAR_TARCOMMON_H

#include <flux/ByteArray.h>

namespace flux {
namespace tar {

unsigned tarHeaderSum(ByteArray *data);

}} // namespace flux::tar

#endif // FLUXTAR_TARCOMMON_H
