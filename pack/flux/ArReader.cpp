/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/UserException.h>
#include <flux/File.h>
#include "ArReader.h"

namespace flux
{

Ref<ArReader> ArReader::open(String path)
{
	return new ArReader(path);
}

ArReader::ArReader(String path)
	: path_(path),
	  file_(File::open(path)),
	  i_(0)
{}

bool ArReader::readHeader(Ref<ArchiveEntry> *nextEntry)
{
	if (i_ == 0) {
		String magic(8);
		i_ = file_->read(magic);
		if (magic != "!<arch>\n")
			throw UserException(path_ + ": unsupported archive format");
	}

	if (!data_) data_ = ByteArray::create(60);
	*nextEntry = ArchiveEntry::create();

	ByteArray *data = data_;
	ArchiveEntry *entry = *nextEntry;

	if (file_->read(data) < data->size()) return false;
	i_ += data->size();
	entry->offset_ = i_;

	if (data->at(58) != 0x60 || data->at(59) != 0x0a)
		throw UserException(path_ + ": unsupported archive format");

	data->scanString(&entry->path_,      " ",  0, 16);
	data->scanInt(&entry->lastModified_,  10, 16, 28);
	data->scanInt(&entry->userId_,        10, 28, 34);
	data->scanInt(&entry->groupId_,       10, 34, 40);
	data->scanInt(&entry->mode_,           8, 40, 48);
	data->scanInt(&entry->size_,          10, 48, 58);

	return true;
}

void ArReader::readData(ArchiveEntry *entry, Stream* sink)
{
	off_t n = entry->size();
	n += n % 2;
	if (sink) file_->transfer(sink, n);
	else file_->skip(n);
	i_ += n;
}

} // namespace flux
