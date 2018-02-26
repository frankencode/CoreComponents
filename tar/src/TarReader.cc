/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/File>
#include <cc/Format>
#include <cc/tar/TarCommon>
#include <cc/tar/TarReader>

namespace cc {
namespace tar {

Ref<TarReader> TarReader::open(Stream *source)
{
    return new TarReader(source);
}

bool TarReader::testFormat(Stream *source)
{
    String data = String::create(512);
    if (source->readSpan(mutate(data)) < data->count()) return false;
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
    if (!data_) data_ = String::create(512);
    *nextEntry = ArchiveEntry::create();

    ByteArray *data = mutate(data_);
    ArchiveEntry *entry = *nextEntry;

    if (source_->readSpan(data) < data->count()) return false;
    i_ += data->count();

    bool eoi = true;
    for (int i = 0; i < data->count(); ++i) {
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
        data->scanNumber(&checksum, 8, 148, 156);
        entry->type_ = data->at(156);
        if (entry->path_ == "")     data->scanString(&entry->path_,     "", 0,   100);
        if (entry->linkPath_ == "") data->scanString(&entry->linkPath_, "", 157, 257);

        probesum = tarHeaderSum(data);
        if (checksum != probesum)
            throw BrokenArchive(i_ - data->count(), Format("Checksum mismatch (%% != %%), path = \"%%\"") << oct(checksum, 6) << oct(probesum, 6) << entry->path());

        if (gnuMagic) {
            while ((entry->type_ == 'K' || entry->type_ == 'L') /*&& entry->path_ == "././@LongLink"*/) {
                data->scanNumber(&entry->size_, 8, 124, 136);
                String longPath = source_->readSpan(entry->size_);
                if (longPath->count() < entry->size_)
                    throw BrokenArchive(i_, "Expected GNU @LongLink data");
                i_ += entry->size_;
                if (entry->size() % 512 != 0) {
                    i_ += source_->skip(512 - entry->size() % 512);
                }
                if (longPath->byteAt(longPath->count() - 1) == 0)
                    mutate(longPath)->truncate(longPath->count() - 1);
                if (entry->type_ == 'K') entry->linkPath_ = longPath;
                else if (entry->type_ == 'L') entry->path_ = longPath;
                if (source_->readSpan(data) < data->count())
                    throw BrokenArchive(i_, "Expected GNU @LongLink header");
                i_ += data->count();
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

    data->scanNumber(&entry->mode_,         8, 100, 108);
    data->scanNumber(&entry->userId_,       8, 108, 116);
    data->scanNumber(&entry->groupId_,      8, 116, 124);
    data->scanNumber(&entry->size_,         8, 124, 136);
    data->scanNumber(&entry->lastModified_, 8, 136, 148);

    if (entry->type() == 0 && entry->path()->count() > 0) {
        if (entry->path()->at(entry->path()->count() - 1) == '/')
            entry->type_ = ArchiveEntry::Directory;
    }

    entry->offset_ = i_;

    return true;
}

void TarReader::readData(ArchiveEntry *entry, Stream* sink)
{
    i_ += source_->transferSpanTo(entry->size(), sink);
    if (entry->size() % 512 != 0)
        i_ += source_->skip(512 - entry->size() % 512);
}

}} // namespace cc::tar
