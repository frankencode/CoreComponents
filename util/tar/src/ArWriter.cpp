/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/str>
#include <flux/File>
#include <flux/tar/ArWriter>

namespace flux {
namespace tar {

Ref<ArWriter> ArWriter::open(Stream *sink)
{
	return new ArWriter(sink);
}

ArWriter::ArWriter(Stream *sink)
	: sink_(sink),
	  firstTime_(true)
{}

void ArWriter::writeFile(String path)
{
	if (firstTime_) {
		firstTime_ = false;
		sink_->write(String("!<arch>\n"));
	}

	Ref<File> file = File::open(path);
	Ref<FileStatus> status = file->status();

	off_t contentSize = status->size();
	if (status->type() != File::Regular) contentSize = 0;

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
	FLUX_ASSERT(header->count() == 60);
	sink_->write(header);
	file->transfer(contentSize, sink_);

	if (contentSize % 2 != 0)
		sink_->write(String("\0", 1));
}

}} // namespace flux::tar
