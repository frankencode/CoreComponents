/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/str.h>
#include <flux/Exception.h>
#include <flux/File.h>
#include "TarCommon.h"
#include "TarWriter.h"

namespace flux
{

Ref<TarWriter> TarWriter::open(Stream *sink)
{
	return new TarWriter(sink);
}

TarWriter::TarWriter(Stream *sink)
	: sink_(sink),
	  hardLinks_(HardLinks::create())
{}

void TarWriter::appendFile(String path)
{
	Ref<File> file = File::open(path);
	Ref<FileStatus> status = FileStatus::read(path, false);

	Ref<StringList> headerFields = StringList::create();

	bool longLink = path->size() >= 100;
	String pathField(100, '\0');
	if (longLink) *pathField = *String("././@LongLink");
	else *pathField = *path;
	headerFields->append(pathField);

	headerFields->append(oct(status->mode(), 8));
	headerFields->append(oct(status->ownerId(), 8));
	headerFields->append(oct(status->groupId(), 8));
	headerFields->append(oct(status->size(), 12));
	headerFields->append(oct(status->st_mtime, 12));

	String checksumField(6, '0');
	headerFields->append(checksumField);
	headerFields->append(String("\0 "));

	String typeField;
	     if (status->type() == File::Regular)     typeField = "0";
	else if (status->type() == File::Directory)   typeField = "5";
	else if (status->type() == File::Symlink)     typeField = "2";
	else if (status->type() == File::CharDevice)  typeField = "3";
	else if (status->type() == File::BlockDevice) typeField = "4";
	else if (status->type() == File::Fifo)        typeField = "6";
	else                                          typeField = "0";

	String linkTarget;
	if (status->numberOfHardLinks() > 1) {
		FileId fid(status);
		if (hardLinks_->lookup(fid, &linkTarget)) typeField = "1";
		else hardLinks_->insert(fid, path);
	}

	String header = headerFields->join();
	unsigned checksum = tarHeaderSum(header);
	*checksumField = *oct(checksum, 6);
	header = headerFields->join();
	sink_->write(header);
}

} // namespace flux
