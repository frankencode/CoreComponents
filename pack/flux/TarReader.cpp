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
#include "TarReader.h"

namespace flux
{

unsigned tarHeaderSum(ByteArray *data)
{
	unsigned sum = 0;
	for (int i = 0;       i < 148; ++i) sum += data->byteAt(i);
	for (int i = 0 + 148; i < 156; ++i) sum += ' ';
	for (int i = 0 + 156; i < 512; ++i) sum += data->byteAt(i);
	return sum;
}

Ref<TarReader> TarReader::open(String path)
{
	return new TarReader(path);
}

TarReader::TarReader(String path)
	: path_(path),
	  file_(File::open(path)),
	  i_(0)
{}

bool TarReader::readHeader(Ref<ArchiveEntry> *nextEntry)
{
	if (!data_) data_ = ByteArray::create(512);
	*nextEntry = ArchiveEntry::create();

	ByteArray *data = data_;
	ArchiveEntry *entry = *nextEntry;

	if (file_->read(data) < data->size()) return false;
	i_ += data->size();

	String magic;
	data->scanString(&magic, "", 257, 263);
	bool ustarMagic = (magic == "ustar");
	bool gnuMagic   = (magic == "ustar ");
	if (!(ustarMagic || gnuMagic)) return false;

	unsigned checksum, probesum;
	data->scanInt(&checksum, 8, 148, 156);
	probesum = tarHeaderSum(data);
	if (checksum != probesum)
		ferr() << "Warning: checksum failure on block " << i_/512 << "(" << oct(checksum) << "!=" << oct(probesum) << ")" << nl;

	entry->type_ = data->at(156);
	data->scanString(&entry->path_,     "", 0,   100);
	data->scanString(&entry->linkPath_, "", 157, 257);

	if (gnuMagic) {
		if ((entry->type_ == 'L' || entry->type_ == 'K') && entry->path_ == "././@LongLink") {
			Ref<StringList> parts = StringList::create();
			while (true) {
				if (file_->read(data) < data->size())
					throw UserException(path_ + ": broken archive, expected GNU @LongLink data");
				i_ += data->size();
				String h;
				data->scanString(&h, "", 0, data->size());
				parts->append(h);
				if (h->size() < data->size()) break;
			}
			if (entry->type_ == 'L') entry->path_ = parts->join();
			else if (entry->type_ == 'K') entry->linkPath_ = parts->join();
			if (file_->read(data) < data->size())
				throw UserException(path_ + ": broken archive, expected GNU @LongLink header");
			i_ += data->size();
			entry->type_ = data->at(156);
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

	entry->offset_ = i_;
	i_ += entry->size_;
	if (i_ % 512 > 0) i_ += 512 - i_ % 512;

	return true;
}

void TarReader::readData(ArchiveEntry *entry, Stream* sink)
{
	off_t n = (entry->size() / 512 + (entry->size() % 512 != 0)) * 512;
	if (sink) i_ += file_->transfer(sink, n);
	else i_ += file_->skip(n);
}

} // namespace flux
