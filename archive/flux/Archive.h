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

#include <flux/Map.h>
#include "ArchiveEntry.h"

namespace flux
{

class Archive: public Map<String, Ref<ArchiveEntry> >
{
public:
	static Ref<Archive> open(String path);
	inline String path() const { return path_; }

private:
	Archive(String path);
	void read();
	void readDenseFormat(File *file, String data);
	void readBlockFormat(File *file, String data);
	String path_;
};

} // namespace flux

#endif // FLUX_ARCHIVE_H
