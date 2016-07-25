/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/tar/ArchiveReader>

namespace cc {
namespace tar {

void ArchiveReader::skipData(ArchiveEntry *entry)
{
    readData(entry);
}

}} // namespace cc::tar
