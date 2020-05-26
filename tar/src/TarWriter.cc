/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/str>
#include <cc/File>
#include <cc/User>
#include <cc/Group>
#include <cc/tar/TarCommon>
#include <cc/tar/TarWriter>

namespace cc {
namespace tar {

Ref<TarWriter> TarWriter::open(Stream *sink)
{
    return new TarWriter{sink};
}

TarWriter::TarWriter(Stream *sink):
    sink_(sink),
    zero_(string("\0", 1)),
    hardLinks_(HardLinks::create()),
    longPathStatus_(FileStatus::create()),
    longLinkStatus_(FileStatus::create())
{}

TarWriter::~TarWriter()
{
    sink_->write(string{1024, '\0'});
}

void TarWriter::writeFile(const string &path)
{
    Ref<FileStatus> status = FileStatus::readHead(path);
    writeFile(path, status);
}

void TarWriter::writeFile(const string &path, const FileStatus *status)
{
    Ref<StringList> headerFields = StringList::create();

    off_t contentSize = status->size();
    if (status->type() != FileType::Regular) contentSize = 0;

    string exactPath = path;
    if (status->type() == FileType::Directory) {
        if (exactPath->count() > 0) {
            if (exactPath->at(exactPath->count() - 1) != '/') exactPath = exactPath + "/";
        }
    }

    string pathField{99, '\0'};
    if (status == longPathStatus_ || status == longLinkStatus_)
        mutate(pathField)->write(string{"././@LongLink"});
    else
        mutate(pathField)->write(exactPath);
    headerFields->append(pathField);
    headerFields->append(zero_);

    headerFields->append(oct(+status->mode(), 7));
    headerFields->append(zero_);
    headerFields->append(oct(status->ownerId(), 7));
    headerFields->append(zero_);
    headerFields->append(oct(status->groupId(), 7));
    headerFields->append(zero_);
    if (status == longPathStatus_ || status == longLinkStatus_)
        headerFields->append(oct(exactPath->count() + 1, 11));
    else
        headerFields->append(oct(contentSize, 11));
    headerFields->append(zero_);
    headerFields->append(oct(status->st_mtime, 11));
    headerFields->append(zero_);

    string checksumField{6, '0'};
    headerFields->append(checksumField);
    headerFields->append(string{"\0 ", 2});

    string typeField, linkTarget;
    if (status == longLinkStatus_ )                       typeField = "K";
    else if (status == longPathStatus_)                   typeField = "L";
    else {
             if (status->type() == FileType::Regular);
        else if (status->type() == FileType::Directory)   typeField = "5";
        else if (status->type() == FileType::Symlink)     typeField = "2";
        else if (status->type() == FileType::CharDevice)  typeField = "3";
        else if (status->type() == FileType::BlockDevice) typeField = "4";
        else if (status->type() == FileType::Fifo)        typeField = "6";
        if (status->numberOfHardLinks() > 1) {
            FileId fid(status);
            if (hardLinks_->lookup(fid, &linkTarget)) typeField = "1";
            else hardLinks_->insert(fid, exactPath);
        }
        else if (status->type() == FileType::Symlink) {
            linkTarget = File::readlink(exactPath);
        }
        if (typeField == "")                          typeField = "0";
        if (typeField != "0") contentSize = 0;
    }
    headerFields->append(typeField);

    string linkField{99, '\0'};
    mutate(linkField)->write(linkTarget);
    headerFields->append(linkField);
    headerFields->append(zero_);

    string gnuMagicField{"ustar  "};
    headerFields->append(gnuMagicField);
    headerFields->append(zero_);

    string userField{31, '\0'};
    mutate(userField)->write(User::lookup(status->ownerId())->name());
    headerFields->append(userField);
    headerFields->append(zero_);

    string groupField{31, '\0'};
    mutate(groupField)->write(Group::lookup(status->groupId())->name());
    headerFields->append(groupField);
    headerFields->append(zero_);

    if (status != longPathStatus_ && status != longLinkStatus_) {
        if (exactPath->count() > pathField->count()) writeFile(exactPath, longPathStatus_);
        if (linkTarget->count() > linkField->count()) writeFile(linkTarget, longLinkStatus_);
    }

    string header = headerFields->join();
    CC_ASSERT(header->count() == 329);
    unsigned checksum = tarHeaderSum(header);
    mutate(checksumField)->write(oct(checksum, 6));
    header = headerFields->join();
    sink_->write(header);
    writePadding(header->count());

    if (status == longPathStatus_ || status == longLinkStatus_) {
        sink_->write(exactPath);
        sink_->write(zero_);
        writePadding(exactPath->count() + 1);
    }
    else if (contentSize > 0) {
        File::open(exactPath)->transferSpanTo(contentSize, sink_);
        writePadding(contentSize);
    }
}

void TarWriter::writePadding(off_t unpaddedSize)
{
    if (unpaddedSize % 512 != 0)
        sink_->write(string{512 - unpaddedSize % 512, '\0'});
}

}} // namespace cc::tar
