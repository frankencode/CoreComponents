/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio.h>
#include <flux/DirWalker.h>
#include "pack.h"

namespace fluxtar
{

void pack(String path, ArchiveWriter *archive, bool verbose)
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

} // namespace fluxtar
