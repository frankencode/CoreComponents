/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/time.h> // utimes
#include <errno.h>
#include <math.h>
#include <cc/exceptions>
#include <cc/File>
#include <cc/FileStatus>

namespace cc {

FileStatus::FileStatus(String path, bool resolve):
    path_(path),
    resolve_(resolve)
{
    isValid_ = update();
}

void FileStatus::setTimes(double lastAccess, double lastModified)
{
    struct timeval tv[2];
    double sec;
    tv[0].tv_usec = modf(lastAccess, &sec) * 1e6;
    tv[0].tv_sec = sec;
    tv[1].tv_usec = modf(lastModified, &sec) * 1e6;
    tv[1].tv_sec = sec;
    int ret = ::utimes(path_, tv);
    if(ret == -1) CC_SYSTEM_ERROR(errno, path_);
}

bool FileStatus::update()
{
    StructStat *buf = static_cast<StructStat *>(this);
    memclr(buf, sizeof(StructStat));
    if (path_ == "")
        return isValid_ = false;
    int ret = resolve_ ? ::stat(path_, buf) : ::lstat(path_, buf);
    if (ret == -1) {
        if ((errno == ENOENT) || (errno == ENOTDIR)) {
            isValid_ = false;
            return false;
        }
        CC_SYSTEM_ERROR(errno, path_);
    }
    isValid_ = true;
    return true;
}

} // namespace cc
