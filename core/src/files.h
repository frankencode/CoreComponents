#pragma once

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace cc {

/// File descriptors of the standard I/O streams
enum StandardStreams {
    StdInFd  = 0, ///< Standard input file descriptor
    StdOutFd = 1, ///< Standard output file descriptor
    StdErrFd = 2  ///< Standard error file descriptor
};

/// Access testing flags
enum AccessFlags {
    ReadOk    = R_OK, ///< Test for read permission
    WriteOk   = W_OK, ///< Test for write permission
    ExecuteOk = X_OK, ///< Test for execution permission
    FileOk    = F_OK  ///< Test if a file (of any type) exists
};

/// %File permission flags
enum ModeFlags {
    SetUserId  = S_ISUID, ///< %Set effective user to file owner on execution
    SetGroupId = S_ISGID, ///< %Set effective group to file owner on execution
    StickyBit  = S_ISVTX,  ///< Allow to read and write, but not delete files when applied to a directory
    UserRead   = 0400, ///< Readable by owner
    UserWrite  = 0200, ///< Writable by owner
    UserExec   = 0100, ///< Executable by owner
    GroupRead  = 0040, ///< Readable by group members
    GroupWrite = 0020, ///< Writable by group members
    GroupExec  = 0010, ///< Executable by group members
    OtherRead  = 0004, ///< Readable by others
    OtherWrite = 0002, ///< Writable by others
    OtherExec  = 0001, ///< Executable by others
    AnyExec    = UserExec|GroupExec|OtherExec
};

/// %File type
enum FileType {
    RegularFileType = S_IFREG, ///< Regular file
    DirectoryType   = S_IFDIR, ///< Directory
    CharDeviceType  = S_IFCHR, ///< Character device
    BlockDeviceType = S_IFBLK, ///< Block device
    NamedPipeType   = S_IFIFO, ///< Named pipe
    SymlinkType     = S_IFLNK, ///< Symbolic link
    SocketType      = S_IFSOCK, ///< Named local socket
};

/// %File open flags
enum OpenFlags {
    OpenReadOnly  = O_RDONLY, ///< Open for reading, only
    OpenWriteOnly = O_WRONLY, ///< Open for writing, only
    OpenReadWrite = O_RDWR,   ///< Open for reading and writing
    OpenAppend    = O_APPEND, ///< Append any write to the end of file
    OpenCreate    = O_CREAT,  ///< Create file if not exists
    OpenTruncate  = O_TRUNC,  ///< Truncate file to size zero
    OpenVirgin    = O_EXCL    ///< Fail to open if file exists already
};

/// Seek method
enum SeekMethod {
    SeekBegin   = SEEK_SET, ///< Seek from the beginning of the file
    SeekCurrent = SEEK_CUR, ///< Seek relative to the current file offset
    SeekEnd     = SEEK_END  ///< Seek relative to the end of file
};

} // namespace cc
