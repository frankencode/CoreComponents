/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/tar/TarWriter>
#include <cc/tar/TarCommon>
#include <cc/File>
#include <cc/User>
#include <cc/Group>
#include <cc/str>

namespace cc {
namespace tar {

Ref<TarWriter> TarWriter::open(Stream *sink)
{
    return new TarWriter{sink};
}

TarWriter::TarWriter(Stream *sink):
    sink_{sink},
    zero_{String{"\0", 1}}
{}

TarWriter::~TarWriter()
{
    sink_->write(String{1024, '\0'});
}

void TarWriter::writeFile(const String &path)
{
    FileStatus status{path, false};
    writeFile(path, status);
}

void TarWriter::writeFile(const String &path, const FileStatus &status)
{
    StringList headerFields;

    off_t contentSize = status->size();
    if (status->type() != FileType::Regular) contentSize = 0;

    String exactPath = path;
    if (status->type() == FileType::Directory) {
        if (exactPath->count() > 0) {
            if (exactPath->at(exactPath->count() - 1) != '/') exactPath = exactPath + "/";
        }
    }

    String pathField{99, '\0'};
    #if 0
    if (status == longPathStatus_ || status == longLinkStatus_)
        mutate(pathField)->write(String{"././@LongLink"});
    else
    #endif
        mutate(pathField)->write(exactPath);
    headerFields->append(pathField);
    headerFields->append(zero_);

    headerFields->append(oct(+status->mode(), 7));
    headerFields->append(zero_);
    headerFields->append(oct(status->ownerId(), 7));
    headerFields->append(zero_);
    headerFields->append(oct(status->groupId(), 7));
    headerFields->append(zero_);
    #if 0
    if (status == longPathStatus_ || status == longLinkStatus_)
        headerFields->append(oct(exactPath->count() + 1, 11));
    else
    #endif
        headerFields->append(oct(contentSize, 11));
    headerFields->append(zero_);
    headerFields->append(oct(status->st_mtime, 11));
    headerFields->append(zero_);

    String checksumField{6, '0'};
    headerFields->append(checksumField);
    headerFields->append(String{"\0 ", 2});

    String typeField, linkTarget;
    #if 0
    if (status == longLinkStatus_ )                       typeField = "K";
    else if (status == longPathStatus_)                   typeField = "L";
    else
    #endif
    {
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

    String linkField{99, '\0'};
    mutate(linkField)->write(linkTarget);
    headerFields->append(linkField);
    headerFields->append(zero_);

    String gnuMagicField{"ustar  "};
    headerFields->append(gnuMagicField);
    headerFields->append(zero_);

    String userField{31, '\0'};
    mutate(userField)->write(User{status->ownerId()}->name());
    headerFields->append(userField);
    headerFields->append(zero_);

    String groupField{31, '\0'};
    mutate(groupField)->write(Group{status->groupId()}->name());
    headerFields->append(groupField);
    headerFields->append(zero_);

    #if 0
    if (status != longPathStatus_ && status != longLinkStatus_) {
        if (exactPath->count() > pathField->count()) writeFile(exactPath, longPathStatus_);
        if (linkTarget->count() > linkField->count()) writeFile(linkTarget, longLinkStatus_);
    }
    #endif

    String header = headerFields->join();
    CC_ASSERT(header->count() == 329);
    unsigned checksum = tarHeaderSum(header);
    mutate(checksumField)->write(oct(checksum, 6));
    header = headerFields->join();
    sink_->write(header);
    writePadding(header->count());

    #if 0
    if (status == longPathStatus_ || status == longLinkStatus_) {
        sink_->write(exactPath);
        sink_->write(zero_);
        writePadding(exactPath->count() + 1);
    }
    else
    #endif
    if (contentSize > 0) {
        File{exactPath}->transferSpanTo(contentSize, sink_);
        writePadding(contentSize);
    }
}

void TarWriter::writePadding(off_t unpaddedSize)
{
    if (unpaddedSize % 512 != 0)
        sink_->write(String{static_cast<int>(512 - unpaddedSize % 512), '\0'});
}

}} // namespace cc::tar
