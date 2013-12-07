/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
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
