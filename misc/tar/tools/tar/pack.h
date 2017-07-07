/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/String>
#include <cc/tar/ArchiveWriter>

namespace cctar {

using namespace cc;
using namespace cc::tar;

void pack(String path, ArchiveWriter *archive, bool verbose);

} // namespace cctar
