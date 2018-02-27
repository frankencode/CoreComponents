/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <zip.h>
#include <cc/zip/exceptions>
#include <cc/zip/ZipFile>

namespace cc {
namespace zip {

int ZipFile::read(CharArray *data)
{
    zip_int64_t ret = zip_fread(file_, data->bytes(), data->count());
    if (ret < 0) throw ZipError(zip_get_error(archive_)->zip_err, zipPath_);
    return ret;
}

}} // namespace cc::zip
