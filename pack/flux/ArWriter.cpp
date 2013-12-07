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
#include "ArWriter.h"

namespace flux
{

Ref<ArWriter> ArWriter::open(Stream *sink)
{
	return new ArWriter(sink);
}

ArWriter::ArWriter(Stream *sink)
	: sink_(sink)
{}

void ArWriter::writeFile(String path)
{
	Ref<File> file = File::open(path);
	Ref<FileStatus> status = file->status();

	Ref<StringList> headerFields = StringList::create();

	String pathField(16, ' ');
	*pathField = *path;
	headerFields->append(pathField);

	String lastModifiedField(12, ' ');
	*lastModifiedField = *dec(status->st_mtime);
	headerFields->append(lastModifiedField);

	String userIdField(6, ' ');
	*userIdField = *dec(status->ownerId());
	headerFields->append(userIdField);

	String groupIdField(6, ' ');
	*groupIdField = *dec(status->groupId());
	headerFields->append(groupIdField);

	String modeField(8, ' ');
	*modeField = *oct(status->mode());
	headerFields->append(modeField);

	String sizeField(10, ' ');
	*sizeField = *dec(status->size());
	headerFields->append(sizeField);

	String magic(2);
	magic->byteAt(0) = 0x60;
	magic->byteAt(1) = 0x0a;
	headerFields->append(magic);

	String header = headerFields->join();
	FLUX_ASSERT(header->size() == 60);
	sink_->write(header);
	file->transfer(status->size(), sink_);
}

} // namespace flux
