/*
 * Copyright (C) 2007-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ZipFile>
#include <cc/ZipStreamState>
#include <zip.h>

namespace cc {

struct ZipFile::State final: public Object::State
{
    explicit State(const String &path):
        path_{path},
        index_{0}
    {
        int errorCode = 0;
        archive_ = zip_open(path, ZIP_RDONLY, &errorCode);
        if (!archive_) throw ZipError{errorCode, path_};
    }

    ~State()
    {
        zip_close(archive_);
    }

    bool read(Out<String> name)
    {
        const char *s = zip_get_name(archive_, index_++, 0);
        if (!s) {
            if (zip_get_error(archive_)->zip_err == ZIP_ER_INVAL) return false;
            throw ZipError{zip_get_error(archive_)->zip_err, path_};
        }
        *name = s;
        return true;
    }

    Stream open(const String &name)
    {
        ZipStream stream{name};
        stream->zipPath_ = path_;
        stream->archive_ = archive_;
        stream->file_ = zip_fopen(archive_, name, 0);
        if (!stream->file_) {
            throw ZipError(zip_get_error(archive_)->zip_err, path_);
        }
        return stream;
    }

    String path_;
    zip_t *archive_ { nullptr };
    uint64_t index_ { 0 };
};

ZipFile::ZipFile(const String &path):
    Object{new State{path}}
{}

String ZipFile::path() const
{
    return me().path_;
}

bool ZipFile::read(Out<String> name)
{
    return me().read(&name);
}

Stream ZipFile::open(const String &name)
{
    return me().open(name);
}

ZipFile::State &ZipFile::me()
{
    return Object::me.as<State>();
}

const ZipFile::State &ZipFile::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
