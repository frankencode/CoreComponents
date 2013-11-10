/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h>
#include <flux/File.h>
#include <flux/UserException.h>
#include "Archive.h"

namespace flux
{

Ref<Archive> Archive::open(String path)
{
	Ref<Archive> archive = new Archive(path);
	archive->openStream();
	return archive;
}

Archive::Archive(String path)
	: path_(path)
{}

void Archive::openStream()
{
	file_ = File::open(path_);
	data_ = file_->map();
	tapeFormat_ = data_->head(8) != "!<arch>\n";
	i_ = tapeFormat_ ? 0 : 8;
}

bool Archive::read(Ref<ArchiveEntry> *entry)
{
	Ref<ArchiveEntry> newEntry = ArchiveEntry::create();
	bool ok = false;
	if (tapeFormat_) ok = readTarHeader(newEntry);
	else ok = readArHeader(newEntry);
	*entry = newEntry;
	return ok;
}

bool Archive::readArHeader(ArchiveEntry *entry)
{
	ByteArray *data = data_;
	int i = i_;
	if (i + 60 > data->size()) return false;
	if (data->at(i + 58) != 0x60 || data->at(i + 59) != 0x0a)
		throw UserException(path_ + ": unsupported archive format");
	data->scanString(&entry->path_,      " ", i, i + 16); i += 16;
	data->scanInt(&entry->lastModified_,  10, i, i + 12); i += 12;
	data->scanInt(&entry->userId_,        10, i, i +  6); i += 6;
	data->scanInt(&entry->groupId_,       10, i, i +  6); i += 6;
	data->scanInt(&entry->mode_,           8, i, i +  8); i += 8;
	data->scanInt(&entry->size_,          10, i, i + 10); i += 10;
	i += 2; // magic, see above
	entry->offset_ = i;
	i += entry->size_;
	i += i % 2;
	i_ = i;
	return true;
}

bool Archive::readTarHeader(ArchiveEntry *entry)
{
	ByteArray *data = data_;
	int i = i_;
	if (i + 512 > data->size()) return false;

	String magic;
	data->scanString(&magic,                 "", i + 257, i + 263);
	if (magic == "") return false;
	bool ustarMagic = (magic == "ustar");
	bool gnuMagic   = (magic == "ustar ");

	unsigned checksum, probesum;
	data->scanInt(&checksum,                  8, i + 148, i + 156);
	probesum = computeChecksum(data, i);
	if (checksum != probesum)
		ferr() << "Warning: checksum failure on block " << i/512 << "(" << oct(checksum) << "!=" << oct(probesum) << nl;

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
			if (i % 512 > 0) i += 512 - i % 512;
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
	data->scanInt(&entry->size_,              8, i + 124, i + 136);
	data->scanInt(&entry->lastModified_,      8, i + 136, i + 148);

	i += 512;
	entry->offset_ = i;
	i += entry->size_;
	if (i % 512 > 0) i += 512 - i % 512;
	i_ = i;

	return true;
}

unsigned Archive::computeChecksum(ByteArray *data, int i0)
{
	unsigned sum = 0;
	for (int i = i0;       i < i0 + 148; ++i) sum += data->byteAt(i);
	for (int i = i0 + 148; i < i0 + 156; ++i) sum += ' ';
	for (int i = i0 + 156; i < i0 + 512; ++i) sum += data->byteAt(i);
	return sum;
}

} // namespace flux
