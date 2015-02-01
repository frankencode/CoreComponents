/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/str>
#include <flux/File>
#include <flux/User>
#include <flux/Group>
#include <flux/tar/TarCommon>
#include <flux/tar/TarWriter>

namespace flux {
namespace tar {

Ref<TarWriter> TarWriter::open(Stream *sink)
{
    return new TarWriter(sink);
}

TarWriter::TarWriter(Stream *sink)
    : sink_(sink),
      zero_(String("\0", 1)),
      hardLinks_(HardLinks::create()),
      longPathStatus_(FileStatus::read()),
      longLinkStatus_(FileStatus::read())
{}

TarWriter::~TarWriter()
{
    sink_->write(String(1024, '\0'));
}

void TarWriter::writeFile(String path)
{
    Ref<FileStatus> status = FileStatus::read(path, false);
    writeFile(path, status);
}

void TarWriter::writeFile(String path, FileStatus *status)
{
    Ref<StringList> headerFields = StringList::create();

    off_t contentSize = status->size();
    if (status->type() != File::Regular) contentSize = 0;

    if (status->type() == File::Directory) {
        if (path->count() > 0) {
            if (path->at(path->count() - 1) != '/') path = path + "/";
        }
    }

    String pathField(99, '\0');
    if (status == longPathStatus_ || status == longLinkStatus_)
        *pathField = *String("././@LongLink");
    else
        *pathField = *path;
    headerFields->append(pathField);
    headerFields->append(zero_);

    headerFields->append(oct(status->mode(), 7));
    headerFields->append(zero_);
    headerFields->append(oct(status->ownerId(), 7));
    headerFields->append(zero_);
    headerFields->append(oct(status->groupId(), 7));
    headerFields->append(zero_);
    if (status == longPathStatus_ || status == longLinkStatus_)
        headerFields->append(oct(path->count() + 1, 11));
    else
        headerFields->append(oct(contentSize, 11));
    headerFields->append(zero_);
    headerFields->append(oct(status->st_mtime, 11));
    headerFields->append(zero_);

    String checksumField(6, '0');
    headerFields->append(checksumField);
    headerFields->append(String("\0 ", 2));

    String typeField, linkTarget;
    if (status == longLinkStatus_ )                   typeField = "K";
    else if (status == longPathStatus_)               typeField = "L";
    else {
             if (status->type() == File::Regular)     ;
        else if (status->type() == File::Directory)   typeField = "5";
        else if (status->type() == File::Symlink)     typeField = "2";
        else if (status->type() == File::CharDevice)  typeField = "3";
        else if (status->type() == File::BlockDevice) typeField = "4";
        else if (status->type() == File::Fifo)        typeField = "6";
        if (status->numberOfHardLinks() > 1) {
            FileId fid(status);
            if (hardLinks_->lookup(fid, &linkTarget)) typeField = "1";
            else hardLinks_->insert(fid, path);
        }
        else if (status->type() == File::Symlink) {
            linkTarget = File::readlink(path);
        }
        if (typeField == "")                          typeField = "0";
        if (typeField != "0") contentSize = 0;
    }
    headerFields->append(typeField);

    String linkField(99, '\0');
    *linkField = *linkTarget;
    headerFields->append(linkField);
    headerFields->append(zero_);

    String gnuMagicField("ustar  ");
    headerFields->append(gnuMagicField);
    headerFields->append(zero_);

    String userField(31, '\0');
    *userField = *User::lookup(status->ownerId())->name();
    headerFields->append(userField);
    headerFields->append(zero_);

    String groupField(31, '\0');
    *groupField = *Group::lookup(status->groupId())->name();
    headerFields->append(groupField);
    headerFields->append(zero_);

    if (status != longPathStatus_ && status != longLinkStatus_) {
        if (path->count() > pathField->count()) writeFile(path, longPathStatus_);
        if (linkTarget->count() > linkField->count()) writeFile(linkTarget, longLinkStatus_);
    }

    String header = headerFields->join();
    FLUX_ASSERT(header->count() == 329);
    unsigned checksum = tarHeaderSum(header);
    *checksumField = *oct(checksum, 6);
    header = headerFields->join();
    sink_->write(header);
    writePadding(header->count());

    if (status == longPathStatus_ || status == longLinkStatus_) {
        sink_->write(path);
        sink_->write(zero_);
        writePadding(path->count() + 1);
    }
    else if (contentSize > 0) {
        File::open(path)->transfer(contentSize, sink_);
        writePadding(contentSize);
    }
}

void TarWriter::writePadding(off_t unpaddedSize)
{
    if (unpaddedSize % 512 != 0)
        sink_->write(String(512 - unpaddedSize % 512, '\0'));
}

}} // namespace flux::tar
