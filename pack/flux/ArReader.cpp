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

namespace flux
{

Ref<ArReader> ArReader::open(Stream *source)
{
	return new ArReader(source);
}

ArReader::ArReader(Stream *source)
	: source_(source),
	  i_(0)
{}

bool ArReader::readHeader(Ref<ArchiveEntry> *nextEntry)
{
	if (i_ == 0) {
		String magic(8);
		source_->readAll(magic);
		if (magic != "!<arch>\n")
			throw BrokenArchive(i_, "Expected ar file header");
		i_ = magic->size();
	}

	if (!data_) data_ = ByteArray::create(60);
	*nextEntry = ArchiveEntry::create();

	ByteArray *data = data_;
	ArchiveEntry *entry = *nextEntry;

	if (source_->readAll(data) < data->size()) return false;

	if (data->at(58) != 0x60 || data->at(59) != 0x0a)
		throw BrokenArchive(i_ + 58, "Expected ar header magic (0x50, 0x0a)");

	data->scanString(&entry->path_,      " ",  0, 16);
	data->scanInt(&entry->lastModified_,  10, 16, 28);
	data->scanInt(&entry->userId_,        10, 28, 34);
	data->scanInt(&entry->groupId_,       10, 34, 40);
	data->scanInt(&entry->mode_,           8, 40, 48);
	data->scanInt(&entry->size_,          10, 48, 58);

	i_ += data->size();
	entry->offset_ = i_;

	return true;
}

void ArReader::readData(ArchiveEntry *entry, Stream* sink)
{
	i_ += source_->transfer(entry->size(), sink);
	if (entry->size() % 2 != 0)
		i_ += source_->skip(entry->size() % 2);
}

} // namespace flux
