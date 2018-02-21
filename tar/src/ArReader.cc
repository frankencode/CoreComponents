/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/File>
#include <cc/tar/ArReader>

namespace cc {
namespace tar {

Ref<ArReader> ArReader::open(Stream *source)
{
    return new ArReader(source);
}

ArReader::ArReader(Stream *source)
    : source_(source),
      i_(0)
{}

bool ArReader::testFormat(Stream *source)
{
    return source->readSpan(8) == "!<arch>\n";
}

bool ArReader::readHeader(Ref<ArchiveEntry> *nextEntry)
{
    if (i_ == 0) {
        if (!testFormat(source_))
            throw BrokenArchive(i_, "Expected ar file header");
        i_ = 8;
    }

    if (!data_) data_ = ByteArray::create(60);
    *nextEntry = ArchiveEntry::create();

    ByteArray *data = data_;
    ArchiveEntry *entry = *nextEntry;

    if (source_->readSpan(data) < data->count()) return false;

    if (data->byteAt(58) != 0x60 || data->byteAt(59) != 0x0a)
        throw BrokenArchive(i_ + 58, "Expected ar header magic (0x50, 0x0a)");

    data->scanString(&entry->path_,         " ",  0, 16);
    data->scanNumber(&entry->lastModified_,  10, 16, 28);
    data->scanNumber(&entry->userId_,        10, 28, 34);
    data->scanNumber(&entry->groupId_,       10, 34, 40);
    data->scanNumber(&entry->mode_,           8, 40, 48);
    data->scanNumber(&entry->size_,          10, 48, 58);

    i_ += data->count();
    entry->offset_ = i_;

    return true;
}

void ArReader::readData(ArchiveEntry *entry, Stream* sink)
{
    i_ += source_->transferSpanTo(entry->size(), sink);
    if (entry->size() % 2 != 0)
        i_ += source_->skip(1);
}

}} // namespace cc::tar
