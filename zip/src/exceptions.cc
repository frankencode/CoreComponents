/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <zip.h>
#include <cc/Format>
#include <cc/exceptions>
#include <cc/zip/exceptions>

namespace cc {
namespace zip {

string ZipError::message() const
{
    switch (errorCode_) {
        case ZIP_ER_EXISTS: return Format("The file %% exists and ZIP_EXCL is set (ZIP_ER_EXISTS)") << zipPath_;
        case ZIP_ER_INCONS: return Format("Inconsistencies were found in file %% (ZIP_ER_INCONS)") << zipPath_;
        case ZIP_ER_INVAL: return "Invalid argument (ZIP_ER_INVAL)";
        case ZIP_ER_MEMORY: return "Memory allocation failed (ZIP_ER_MEMORY)";
        case ZIP_ER_NOZIP: return Format("File %% is not a ZIP file (ZIP_ER_NOZIP)") << zipPath_;
        case ZIP_ER_OPEN: return Format("File %% could not be opened (ZIP_ER_OPEN)") << zipPath_;
        case ZIP_ER_READ: return Format("Read error: %% (ZIP_ER_READ)") << systemError(errno);
        case ZIP_ER_SEEK: return Format("File %% is not seekable (ZIP_ER_SEEK)") << zipPath_;
        case ZIP_ER_DELETED: return "Referenced file has been deleted (ZIP_ER_DELETED)";
        case ZIP_ER_NOENT: return "File not found (ZIP_ER_NOENT)";
        case ZIP_ER_CHANGED: return "Orginal data lost (ZIP_ER_CHANGED)";
        case ZIP_ER_COMPNOTSUPP: return "Compression method is not supported (ZIP_ER_COMPNOTSUPP)";
        case ZIP_ER_ENCRNOTSUPP: return "Encryption method is not supported (ZIP_ER_ENCRNOTSUPP)";
        case ZIP_ER_NOPASSWD: return "Cannot read encrypted file without password (ZIP_ER_NOPASSWD)";
        case ZIP_ER_WRONGPASSWD: return "Invalid password (ZIP_ER_WRONGPASSWD)";
        case ZIP_ER_ZLIB: return "zlib stream initialization failed (ZIP_ER_ZLIB)";
    };
    return "Zip error";
}

}} // namespace cc::zip
