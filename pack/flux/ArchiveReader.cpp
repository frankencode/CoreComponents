/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/File.h>
#include "ArReader.h"
#include "TarReader.h"
#include "ArchiveReader.h"

namespace flux
{

Ref<ArchiveReader> ArchiveReader::open(String path)
{
	Ref<File> file = File::open(path);
	if (path->suffix() == "a" || path->suffix() == "deb") return ArReader::open(file);
	return TarReader::open(file);
}

void ArchiveReader::skipData(ArchiveEntry *entry)
{
	readData(entry);
}

} // namespace flux
