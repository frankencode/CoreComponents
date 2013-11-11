/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_ARCHIVE_H
#define FLUX_ARCHIVE_H

#include <flux/File.h>
#include "ArchiveEntry.h"

namespace flux
{

class Archive: public Object
{
public:
	static Ref<Archive> open(String path);
	inline String path() const { return path_; }
	inline File *file() const { return file_; }
	inline ByteArray *data() const { return data_; }

	bool read(Ref<ArchiveEntry> *entry);

private:
	Archive(String path);

	void openStream();
	bool readArHeader(ArchiveEntry *entry);
	bool readTarHeader(ArchiveEntry *entry);

	static unsigned computeChecksum(ByteArray *data, int i0);

	String path_;
	Ref<File> file_;
	String data_;
	bool tapeFormat_;
	off_t i_;
};

} // namespace flux

#endif // FLUX_ARCHIVE_H
