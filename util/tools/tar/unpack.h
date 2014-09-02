/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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
