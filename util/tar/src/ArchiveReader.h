/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTAR_ARCHIVEREADER_H
#define FLUXTAR_ARCHIVEREADER_H

#include <flux/Stream.h>
#include "ArchiveEntry.h"

namespace flux {
namespace tar {

class BrokenArchive {
public:
	BrokenArchive(off_t offset, String reason)
		: offset_(offset), reason_(reason)
	{}

	inline off_t offset() const { return offset_; }
	inline String reason() const { return reason_; }

private:
	off_t offset_;
	String reason_;
};

class UnsupportedArchiveFormat {};

class ArchiveReader: public Object
{
public:
	virtual bool readHeader(Ref<ArchiveEntry> *entry) = 0;
	virtual void readData(ArchiveEntry *entry, Stream *sink = 0) = 0;
	void skipData(ArchiveEntry *entry);
};

}} // namespace flux::tar

#endif // FLUXTAR_ARCHIVEREADER_H
