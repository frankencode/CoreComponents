/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <zip.h>
#include <cc/zip/ZipFile>
#include <cc/zip/ZipArchive>

namespace cc {
namespace zip {

Ref<ZipArchive> ZipArchive::open(String path)
{
    return new ZipArchive(path);
}

ZipArchive::ZipArchive(String path):
    path_(path),
    index_(0)
{
    int errorCode = 0;
    archive_ = zip_open(path, ZIP_RDONLY, &errorCode);
    if (!archive_) throw ZipError(errorCode, path_);
}

ZipArchive::~ZipArchive()
{
    if (zip_close(archive_) == -1)
        throw ZipError(zip_get_error(archive_)->zip_err, path_);
}

String ZipArchive::path() const { return path_; }

bool ZipArchive::read(String *name)
{
    const char *s = zip_get_name(archive_, index_++, 0);
    if (!s) {
        if (zip_get_error(archive_)->zip_err == ZIP_ER_INVAL) return false;
        throw ZipError(zip_get_error(archive_)->zip_err, path_);
    }
    *name = s;
    return true;
}

Ref<Stream> ZipArchive::openFile(String name)
{
    Ref<ZipFile> stream = new ZipFile(name);
    stream->zipPath_ = path_;
    stream->archive_ = archive_;
    stream->file_ = zip_fopen(archive_, name, 0);
    if (!stream->file_)
        throw ZipError(zip_get_error(archive_)->zip_err, path_);
    return stream;
}

}} // namespace cc::zip
