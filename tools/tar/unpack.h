/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTAR_UNPACK_H
#define FLUXTAR_UNPACK_H

#include <flux/tar/ArchiveReader.h>

namespace fluxtar
{

using namespace flux;
using namespace flux::tar;

void list(ArchiveReader *archive);
void status(ArchiveReader *archive);
void unpack(ArchiveReader *archive, bool verbose);

} // namespace fluxtar

#endif // FLUXTAR_UNPACK_H
