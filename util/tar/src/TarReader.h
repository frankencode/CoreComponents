/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTAR_TARREADER_H
#define FLUXTAR_TARREADER_H

#include "ArchiveReader.h"

namespace flux {
namespace tar {

class TarReader: public ArchiveReader
{
public:
	static Ref<TarReader> open(Stream *source);

	static bool testFormat(Stream *source);

	bool readHeader(Ref<ArchiveEntry> *entry);
	void readData(ArchiveEntry *entry, Stream* sink = 0);

private:
	TarReader(Stream *source);

	Ref<Stream> source_;
	Ref<ByteArray> data_;
	off_t i_;
};

}} // namespace flux::tar

#endif // FLUXTAR_TARREADER_H
