/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/str>
#include <cc/File>
#include <cc/tar/ArWriter>

namespace cc {
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
    Ref<FileStatus> status = FileStatus::read(path);

    off_t contentSize = status->size();
    if (status->type() != FileType::Regular) contentSize = 0;

    Ref<StringList> headerFields = StringList::create();

    String pathField(16, ' ');
    mutate(pathField)->write(path);
    headerFields->append(pathField);

    String lastModifiedField(12, ' ');
    mutate(lastModifiedField)->write(dec(status->st_mtime));
    headerFields->append(lastModifiedField);

    String userIdField(6, ' ');
    mutate(userIdField)->write(dec(status->ownerId()));
    headerFields->append(userIdField);

    String groupIdField(6, ' ');
    mutate(groupIdField)->write(dec(status->groupId()));
    headerFields->append(groupIdField);

    String modeField(8, ' ');
    mutate(modeField)->write(oct(status->mode()));
    headerFields->append(modeField);

    String sizeField(10, ' ');
    mutate(sizeField)->write(dec(status->size()));
    headerFields->append(sizeField);

    String magic(2);
    mutate(magic)->byteAt(0) = 0x60;
    mutate(magic)->byteAt(1) = 0x0a;
    headerFields->append(magic);

    String header = headerFields->join();
    CC_ASSERT(header->count() == 60);
    sink_->write(header);
    file->transferSpanTo(contentSize, sink_);

    if (contentSize % 2 != 0)
        sink_->write(String("\0", 1));
}

}} // namespace cc::tar
