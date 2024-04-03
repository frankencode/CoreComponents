/*
 * Copyright (C) 2007-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ZipStreamState>
#include <cc/SystemError>
#include <cc/Format>

namespace cc {

ZipStream::ZipStream(const String &path):
    Stream{new State{path}}
{}

ZipStream::State *ZipStream::operator->()
{
    return &Object::me.as<State>();
}

String ZipError::message() const
{
    String s;

    switch (errorCode_) {
        case ZIP_ER_EXISTS: s = Format{"The file %% exists and ZIP_EXCL is set (ZIP_ER_EXISTS)"} << zipPath_; break;
        case ZIP_ER_INCONS: s = Format{"Inconsistencies were found in file %% (ZIP_ER_INCONS)"} << zipPath_; break;
        case ZIP_ER_INVAL: s = "Invalid argument (ZIP_ER_INVAL)"; break;
        case ZIP_ER_MEMORY: s = "Memory allocation failed (ZIP_ER_MEMORY)"; break;
        case ZIP_ER_NOZIP: s = Format{"File %% is not a ZIP file (ZIP_ER_NOZIP)"} << zipPath_; break;
        case ZIP_ER_OPEN: s = Format{"File %% could not be opened (ZIP_ER_OPEN)"} << zipPath_; break;
        case ZIP_ER_READ: s = Format{"Read error: %% (ZIP_ER_READ)"} << systemError(errno); break;
        case ZIP_ER_SEEK: s = Format{"File %% is not seekable (ZIP_ER_SEEK)"} << zipPath_; break;
        case ZIP_ER_DELETED: s = "Referenced file has been deleted (ZIP_ER_DELETED)"; break;
        case ZIP_ER_NOENT: s = "File not found (ZIP_ER_NOENT)"; break;
        case ZIP_ER_CHANGED: s = "Orginal data lost (ZIP_ER_CHANGED)"; break;
        case ZIP_ER_COMPNOTSUPP: s = "Compression method is not supported (ZIP_ER_COMPNOTSUPP)"; break;
        case ZIP_ER_ENCRNOTSUPP: s = "Encryption method is not supported (ZIP_ER_ENCRNOTSUPP)"; break;
        case ZIP_ER_NOPASSWD: s = "Cannot read encrypted file without password (ZIP_ER_NOPASSWD)"; break;
        case ZIP_ER_WRONGPASSWD: s = "Invalid password (ZIP_ER_WRONGPASSWD)"; break;
        case ZIP_ER_ZLIB: s = "zlib stream initialization failed (ZIP_ER_ZLIB)"; break;
        default: s = "Zip error"; break;
    };

    return s;
}

} // namespace cc
