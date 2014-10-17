/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTAR_ARREADER_H
#define FLUXTAR_ARREADER_H

#include <flux/tar/ArchiveReader>

namespace flux {
namespace tar {

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

}} // namespace flux::tar

#endif // FLUXTAR_ARREADER_H
