/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXPACK_UNPACK_H
#define FLUXPACK_UNPACK_H

#include <flux/ArchiveReader.h>

namespace fluxpack
{

using namespace flux;

void list(ArchiveReader *archive);
void status(ArchiveReader *archive);
void unpack(ArchiveReader *archive);

} // namespace fluxpack

#endif // FLUXPACK_UNPACK_H
