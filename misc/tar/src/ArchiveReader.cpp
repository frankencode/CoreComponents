/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/tar/ArchiveReader>

namespace flux {
namespace tar {

void ArchiveReader::skipData(ArchiveEntry *entry)
{
    readData(entry);
}

}} // namespace flux::tar
