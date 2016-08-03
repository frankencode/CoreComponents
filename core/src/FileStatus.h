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
    /** Read file status
      * \param path file or directory path
      * \param resolve follow symlinks (true) or read status of the symlink file itself (false)
      * \return new object instance
      */
    inline static Ref<FileStatus> read(String path = "", bool resolve = true) { return new FileStatus(path, resolve); }

    /** Read file status without resolving symlinks
      * \param path file or directory path
      * \return new object instance
      */
    inline static Ref<FileStatus> readUnresolved(String path) { return new FileStatus(path, false); }

    /// File or directory path
    inline String path() const { return path_; }

    /// File type
    inline FileType type() const  { return FileType(st_mode & S_IFMT); }

    /// File access mask
    inline int mode() const { return st_mode & (~S_IFMT); }

    /// File size in number of bytes
    inline off_t size() const { return st_size; }

    /// File size in number of blocks
    inline off_t sizeInBlocks() const { return st_blocks; }

    /// Block size
    inline off_t sizeOfBlock() const { return st_blksize; }

    /// File owner user ID
    inline uid_t ownerId() const { return st_uid; }

    /// File owner group ID
    inline gid_t groupId() const { return st_gid; }

    /// Time when file was last accessed
    inline double lastAccess() const { return st_atime; }

    /// Time when file was last modified
    inline double lastModified() const { return st_mtime; }

    /// Time when file meta information was last modified
    inline double lastChanged() const { return st_ctime; }

    /** Set the file times
      * \lastAccess time when file was last accessed
      * \lastModified time when file was last modified
      */
    void setTimes(double lastAccess, double lastModified);

    /// ID of devices containing the file
    inline dev_t storageId() const { return st_dev; }

    /// Inode number
    inline ino_t inodeNumber() const { return st_ino; }

    /// Mumber of hard links (number of directory entries)
    inline nlink_t numberOfHardLinks() const { return st_nlink; }

    /// Device ID (if device file)
    inline dev_t deviceId() const { return st_rdev; }

    /// Returns true if file exists otherwise false
    inline bool exists() const { return exists_; }

private:
    FileStatus(String path, bool resolve = true);

    bool update();

    String path_;
    bool exists_;
    bool resolve_;
};

} // namespace cc
