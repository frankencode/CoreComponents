/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_ARREADER_H
#define FLUX_ARREADER_H

#include "ArchiveReader.h"

namespace flux
{

class ArReader: public ArchiveReader
{
public:
	static Ref<ArReader> open(Stream *source);

	static bool testFormat(Stream *source);

	bool readHeader(Ref<ArchiveEntry> *entry);
	void readData(ArchiveEntry *entry, Stream* sink = 0);

private:
	ArReader(Stream *source);

	Ref<Stream> source_;
	Ref<ByteArray> data_;
	off_t i_;
};

} // namespace flux

#endif // FLUX_ARREADER_H
