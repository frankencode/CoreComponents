/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/File.h>
#include <flux/UserException.h>
#include "Archive.h"

namespace flux
{

Ref<Archive> Archive::open(String path)
{
	Ref<Archive> archive = new Archive(path);
	archive->read();
	return archive;
}

void Archive::read()
{
	Ref<File> file = File::open(path_);
	String data = file->map();

	if (data->head(8) == "!<arch>\n") {
		readDenseFormat(file, data);
		return;
	}

	readBlockFormat(file, data);
}

void Archive::readDenseFormat(File *file, String data)
{
	for (int i = 8; i + 60 <= data->size();) {
		if (data->at(i + 58) != 0x60 || data->at(i + 59) != 0x0a)
			throw UserException(path_ + ": unsupported archive format");
		Ref<ArchiveEntry> entry = ArchiveEntry::create();
		off_t size = 0;
		data->scanString(&entry->path_,      " ", i, i + 16); i += 16;
		data->scanInt(&entry->lastModified_,  10, i, i + 12); i += 12;
		data->scanInt(&entry->userId_,        10, i, i +  6); i += 6;
		data->scanInt(&entry->groupId_,       10, i, i +  6); i += 6;
		data->scanInt(&entry->mode_,           8, i, i +  8); i += 8;
		data->scanInt(&size,                  10, i, i + 10); i += 10;
		i += 2; // magic, see above
		entry->data_ = file->map(i, i + size); i += size;
		i += i % 2;
		insert(entry->path_, entry);
	}
}

void Archive::readBlockFormat(File *file, String data)
{
	for (int i = 0; i + 512 <= data->size();)
	{
		Ref<ArchiveEntry> entry = ArchiveEntry::create();
		int size = 0;
		String magic;
		data->scanString(&magic,                 "", i + 257, i + 263);
		if (magic == "") break;
		bool ustarMagic = (magic == "ustar");
		bool gnuMagic   = (magic == "ustar ");
		entry->type_ =                      data->at(i + 156);
		data->scanString(&entry->path_,          "", i      , i + 100);
		data->scanString(&entry->linkPath_,      "", i + 157, i + 257);
		if (gnuMagic) {
			if ((entry->type_ == 'L' || entry->type_ == 'K') && entry->path_ == "././@LongLink") {
				i += 512;
				if (i + 512 > data->size())
					throw UserException(path_ + ": broken archive, expected GNU @LongLink data");
				String h;
				data->scanString(&h, "", i, data->size());
				if (entry->type_ == 'L') entry->path_ = h;
				else if (entry->type_ == 'K') entry->linkPath_ = h;
				i += h->size() + 1;
				i += i % 512;
				if (i + 512 > data->size())
					throw UserException(path_ + ": broken archive, expected GNU @LongLink header");
				entry->type_ =              data->at(i + 156);
			}
		}
		if (ustarMagic || gnuMagic) {
			String prefix;
			data->scanString(&entry->userName_,  "", i + 265, i + 297);
			data->scanString(&entry->groupName_, "", i + 297, i + 329);
			if (!gnuMagic) {
				data->scanString(&prefix,        "", i + 345, i + 500);
				if (prefix != "") entry->path_ = prefix + "/" + entry->path_;
			}
		}
		data->scanInt(&entry->mode_,              8, i + 100, i + 108);
		data->scanInt(&entry->userId_,            8, i + 108, i + 116);
		data->scanInt(&entry->groupId_,           8, i + 116, i + 124);
		data->scanInt(&size,                      8, i + 124, i + 136);
		data->scanInt(&entry->lastModified_,      8, i + 136, i + 148);
		i += 512;
		if (i + 512 > data->size())
			throw UserException(path_ + ": broken archive, expected file data");
		entry->data_ = file->map(i, i + size);
		i += size;
		i += i % 512;
		insert(entry->path_, entry);
	}
}

} // namespace flux
