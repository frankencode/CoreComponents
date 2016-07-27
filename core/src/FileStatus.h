/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <cc/files>
#include <cc/String>
#include <cc/SystemStream>

namespace cc {

class File;

typedef struct stat StructStat;

/** \class FileStatus FileStatus.h cc/FileStatus
  * \brief Read %File attributes
  * \see File
  */
class FileStatus: public StructStat, public Object
{
public:
    inline static Ref<FileStatus> read(String path = "", bool resolve = true) { return new FileStatus(path, resolve); }
    inline static Ref<FileStatus> readUnresolved(String path) { return new FileStatus(path, false); }

    inline String path() const { return path_; }

    inline FileType type() const  { return FileType(st_mode & S_IFMT); }
    inline int mode() const { return st_mode & (~S_IFMT); }

    /// file size in number of bytes
    inline off_t size() const { return st_size; }

    /// file size in number of blocks
    inline off_t sizeInBlocks() const { return st_blocks; }

    /// block size
    inline off_t sizeOfBlock() const { return st_blksize; }

    inline uid_t ownerId() const { return st_uid; }
    inline gid_t groupId() const { return st_gid; }

    inline double lastAccess() const { return st_atime; }
    inline double lastModified() const { return st_mtime; }
    inline double lastChanged() const { return st_ctime; }
    void setTimes(double lastAccess, double lastModified);

    inline dev_t storageId() const { return st_dev; }
    inline ino_t inodeNumber() const { return st_ino; }
    inline nlink_t numberOfHardLinks() const { return st_nlink; }

    inline dev_t deviceId() const { return st_rdev; }

    inline bool exists() const { return exists_; }

private:
    FileStatus(String path, bool resolve = true);

    bool update();

    String path_;
    bool exists_;
    bool resolve_;
};

} // namespace cc
