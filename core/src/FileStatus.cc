/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/FileStatus>
#include <cstring>
#include <cmath>

namespace cc {

FileStatus::FileStatus(const String &path, bool followSymlink)
{
    me().path = path;
    me().followSymlink = followSymlink;
    update();
}

void FileStatus::update()
{
    StructStat &buf = static_cast<StructStat &>(me());
    std::memset(&buf, 0, sizeof(StructStat));
    if (me().path == "") {
        me().isValid = false;
    }
    else {
        int ret = me().followSymlink ? ::stat(me().path, &buf) : ::lstat(me().path, &buf);
        if (ret == -1) {
            if ((errno == ENOENT) || (errno == ENOTDIR))
                me().isValid = false;
            else
                CC_SYSTEM_ERROR(errno, me().path);
        }
        else
            me().isValid = true;
    }
}

void FileStatus::setTimes(double lastAccess, double lastModified)
{
    struct timespec tv[2];
    double sec;
    tv[0].tv_nsec = std::modf(lastAccess, &sec) * 1e9;
    tv[0].tv_sec = sec;
    tv[1].tv_nsec = std::modf(lastModified, &sec) * 1e9;
    tv[1].tv_sec = sec;
    if (::utimensat(AT_FDCWD, me().path, tv, AT_SYMLINK_NOFOLLOW * (!me().followSymlink)) == -1)
        CC_SYSTEM_ERROR(errno, me().path);

    me().st_atim = tv[0];
    me().st_mtim = tv[1];
}

} // namespace cc
