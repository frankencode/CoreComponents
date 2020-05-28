/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/DirWalker>
#include "pack.h"

namespace cctar {

void pack(const String &path, ArchiveWriter *archive, bool verbose)
{
    if (verbose) ferr() << path << nl;
    archive->writeFile(path);

    Ref<DirWalker> walker = DirWalker::tryOpen(path);
    if (!walker) return;

    walker->setFollowSymlink(true);
    String sourcePath;
    while (walker->read(&sourcePath)) {
        if (verbose) ferr() << sourcePath << nl;
        archive->writeFile(sourcePath);
    }
}

} // namespace cctar
