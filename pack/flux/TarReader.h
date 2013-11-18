/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_TARREADER_H
#define FLUX_TARREADER_H

#include "ArchiveReader.h"

namespace flux
{

class File;

class TarReader: public ArchiveReader
{
public:
	static Ref<TarReader> open(String path);

	bool readHeader(Ref<ArchiveEntry> *entry);
	void readData(ArchiveEntry *entry, Stream* sink = 0);

private:
	TarReader(String path);

	String path_;
	Ref<File> file_;
	Ref<ByteArray> data_;
	off_t i_;
};

} // namespace flux

#endif // FLUX_TARREADER_H
