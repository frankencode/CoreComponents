/*
 * Copyright (C) 2020-2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/FileInfo>
#include <cstring>
#include <cmath>

namespace cc {

using StructStat = struct stat;

struct FileInfo::State final: public Object::State, public StructStat
{
    State(const String &path, bool followSymlink):
        path_{path},
        isValid_{path != ""}
    {
        StructStat *buf = static_cast<StructStat *>(this);
        std::memset(buf, 0, sizeof(StructStat));
        if (path != "") {
            int ret = followSymlink ? ::stat(path, buf) : ::lstat(path, buf);
            if (ret == -1) {
                if ((errno == ENOENT) || (errno == ENOTDIR))
                    isValid_ = false;
                else
                    CC_SYSTEM_ERROR(errno, path);
            }
        }
    }

    String path_;
    bool isValid_;
};

FileInfo::FileInfo(const String &path, bool followSymlink):
    Object{new State{path, followSymlink}}
{}

String FileInfo::path() const
{
    return me().path_;
}

bool FileInfo::isValid() const
{
    return Object::me && me().isValid_;
}

FileType FileInfo::type() const
{
    return static_cast<FileType>(me().st_mode & S_IFMT);
}

FileMode FileInfo::mode() const
{
    return static_cast<FileMode>(me().st_mode & (~S_IFMT));
}

long long FileInfo::size() const
{
    return me().st_size;
}

long long FileInfo::blockCount() const
{
    return me().st_blocks;
}

long FileInfo::blockSize() const
{
    return me().st_blksize;
}

int FileInfo::ownerId() const
{
    return me().st_uid;
}

int FileInfo::groupId() const
{
    return me().st_gid;
}

double FileInfo::lastAccess() const
{
    return me().st_atim.tv_sec + me().st_atim.tv_nsec / 1e9;
}

double FileInfo::lastModified() const
{
    return me().st_mtim.tv_sec + me().st_mtim.tv_nsec / 1e9;
}

double FileInfo::lastChanged() const
{
    return me().st_ctim.tv_sec + me().st_ctim.tv_nsec / 1e9;
}

int FileInfo::storageId() const
{
    return me().st_dev;
}

long FileInfo::iNodeNumber() const
{
    return me().st_ino;
}

long FileInfo::linkCount() const
{
    return me().st_nlink;
}

int FileInfo::deviceId() const
{
    return me().st_rdev;
}

const FileInfo::State &FileInfo::fallback() const
{
    static FileInfo::State fallbackState {"", false};
    return fallbackState;
}

const FileInfo::State &FileInfo::me() const
{
    if (!Object::me) return fallback();
    return Object::me.as<State>();
}

} // namespace cc
