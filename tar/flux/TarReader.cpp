/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/File.h>
#include <flux/Format.h>
#include "TarCommon.h"
#include "TarReader.h"

namespace flux
{

Ref<TarReader> TarReader::open(Stream *source)
{
	return new TarReader(source);
}

bool TarReader::testFormat(Stream *source)
{
	Ref<ByteArray> data = ByteArray::create(512);
	if (source->readAll(data) < data->size()) return false;
	String magic;
	data->scanString(&magic, "", 257, 263);
	return magic == "ustar " || magic == "ustar";
}

TarReader::TarReader(Stream *source)
	: source_(source),
	  i_(0)
{}

bool TarReader::readHeader(Ref<ArchiveEntry> *nextEntry)
{
	if (!data_) data_ = ByteArray::create(512);
	*nextEntry = ArchiveEntry::create();

	ByteArray *data = data_;
	ArchiveEntry *entry = *nextEntry;

	if (source_->readAll(data) < data->size()) return false;
	i_ += data->size();

	bool eoi = true;
	for (int i = 0; i < data->size(); ++i) {
		if (data->byteAt(i) != 0) {
			eoi = false;
			break;
		}
	}

	if (eoi) return false;

	bool ustarMagic = false;
	bool gnuMagic = false;

	bool readAgain = true;
	while (readAgain) {
		readAgain = false;

		String magic;
		data->scanString(&magic, "", 257, 263);
		ustarMagic = (magic == "ustar");
		gnuMagic   = (magic == "ustar ");

		unsigned checksum, probesum;
		data->scanInt(&checksum, 8, 148, 156);
		entry->type_ = data->at(156);
		if (entry->path_ == "")     data->scanString(&entry->path_,     "", 0,   100);
		if (entry->linkPath_ == "") data->scanString(&entry->linkPath_, "", 157, 257);

		probesum = tarHeaderSum(data);
		if (checksum != probesum)
			throw BrokenArchive(i_ - data->size(), Format("Checksum mismatch (%% != %%), path = \"%%\"") << oct(checksum, 6) << oct(probesum, 6) << entry->path());

		if (gnuMagic) {
			while ((entry->type_ == 'K' || entry->type_ == 'L') /*&& entry->path_ == "././@LongLink"*/) {
				data->scanInt(&entry->size_, 8, 124, 136);
				String longPath = source_->readAll(entry->size_);
				if (longPath->size() < entry->size_)
					throw BrokenArchive(i_, "Expected GNU @LongLink data");
				i_ += entry->size_;
				if (entry->size() % 512 != 0) {
					i_ += source_->skip(512 - entry->size() % 512);
				}
				if (longPath->byteAt(longPath->size() - 1) == 0)
					longPath->truncate(longPath->size() - 1);
				if (entry->type_ == 'K') entry->linkPath_ = longPath;
				else if (entry->type_ == 'L') entry->path_ = longPath;
				if (source_->readAll(data) < data->size())
					throw BrokenArchive(i_, "Expected GNU @LongLink header");
				i_ += data->size();
				entry->type_ = data->at(156);
				readAgain = true;
			}
		}
	}

	if (ustarMagic || gnuMagic) {
		String prefix;
		data->scanString(&entry->userName_,  "", 265, 297);
		data->scanString(&entry->groupName_, "", 297, 329);
		if (!gnuMagic) {
			data->scanString(&prefix,        "", 345, 500);
			if (prefix != "") entry->path_ = prefix + "/" + entry->path_;
		}
	}

	data->scanInt(&entry->mode_,         8, 100, 108);
	data->scanInt(&entry->userId_,       8, 108, 116);
	data->scanInt(&entry->groupId_,      8, 116, 124);
	data->scanInt(&entry->size_,         8, 124, 136);
	data->scanInt(&entry->lastModified_, 8, 136, 148);

	if (entry->type() == 0 && entry->path()->size() > 0) {
		if (entry->path()->at(entry->path()->size() - 1) == '/')
			entry->type_ = ArchiveEntry::Directory;
	}

	entry->offset_ = i_;

	return true;
}

void TarReader::readData(ArchiveEntry *entry, Stream* sink)
{
	i_ += source_->transfer(entry->size(), sink);
	if (entry->size() % 512 != 0)
		i_ += source_->skip(512 - entry->size() % 512);
}

} // namespace flux
