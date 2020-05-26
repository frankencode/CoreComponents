/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/string>
#include <cc/tar/ArchiveWriter>

namespace cctar {

using namespace cc;
using namespace cc::tar;

void pack(const string &path, ArchiveWriter *archive, bool verbose);

} // namespace cctar
