/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/tar/ArchiveReader>

namespace cctar {

using namespace cc;
using namespace cc::tar;

void list(ArchiveReader *archive);
void status(ArchiveReader *archive);
void unpack(ArchiveReader *archive, bool verbose);

} // namespace cctar

