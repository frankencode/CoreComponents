/*
 * Copyright (C) 2020-2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/File>
#include <cc/FileInfo>
#include <cc/Dir>
#include <cc/Random>
#include <cc/str>

namespace cc {

long long File::State::transferTo(const Stream &sink, long long count, const Bytes &buffer)
{
    if (!sink && count > 0) {
        off_t ret = 0;
        if (count > 0) ret = ::lseek(fd_, count, SEEK_CUR);
        else ret = ::lseek(fd_, 0, SEEK_END);
        if (ret != -1) return count;
    }
    //! \todo make use of memory mapped I/O if applicable
    return IoStream::State::transferTo(sink, count, buffer);
}

bool File::access(const String &path, FileAccess mode)
{
    return (path != "") && ::access(path, +mode) == 0;
}

bool File::exists(const String &path)
{
    return access(path, FileAccess::Exists);
}

void File::create(const String &path, FileMode mode)
{
    int fd = ::open(path, O_RDONLY|O_CREAT|O_EXCL, +mode);
    if (fd == -1) CC_SYSTEM_RESOURCE_ERROR(errno, path);
    ::close(fd);
}

void File::establish(const String &path, FileMode fileMode, FileMode dirMode)
{
    if (path.find('/'))
        Dir::establish(path.cdUp(), dirMode);
    if (!File::exists(path))
        File::create(path, fileMode);
}

String File::createUnique(const String &path, FileMode mode, char placeHolder)
{
    assert(path.find(placeHolder));

    String candidate;

    for (Random random; true;) {
        candidate = path.copy();
        for (long i = 0, n = candidate.count(); i < n; ++i) {
            if (candidate[i] == placeHolder) {
                char r = static_cast<char>(random.get(0, 61));
                if ((0 <= r) && (r <= 9))
                    r += '0';
                else if ((10 <= r) && (r <= 35))
                    r += 'a' - 10;
                else if ((36 <= r) && (r <= 61))
                    r += 'A' - 36;
                candidate[i] = r;
            }
        }
        int fd = ::open(candidate, O_RDONLY|O_CREAT|O_EXCL, mode);
        if (fd == -1) {
            if (errno != EEXIST)
                CC_SYSTEM_RESOURCE_ERROR(errno, candidate);
        }
        else {
            ::close(fd);
            break;
        }
    }

    return candidate;
}

String File::createTemp(FileMode mode)
{
    return createUnique("/tmp/" + str(::getpid()) + "_########", mode, '#');
}

void File::chown(const String &path, uid_t ownerId, gid_t groupId)
{
    if (::chown(path, ownerId, groupId) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

void File::rename(const String &oldPath, const String &newPath)
{
    if (::rename(oldPath, newPath) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, oldPath);
}

void File::link(const String &oldPath, const String &newPath)
{
    if (::link(oldPath, newPath) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, newPath);
}

void File::unlink(const String &path)
{
    if (::unlink(path) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

void File::symlink(const String &path, const String &newPath)
{
    if (::symlink(path, newPath) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

String File::readlink(const String &path)
{
    String buf = String::allocate(128);
    while (true) {
        long n = ::readlink(path, buf, buf.count());
        if (n == -1) return String{};
        if (n <= buf.count()) {
            buf.truncate(n);
            break;
        }
        buf = String::allocate(n);
    }
    return buf;
}

void File::clean(const String &path)
{
    FileInfo status{path};
    if (status.isValid()) {
        if (status.type() == FileType::Directory) {
            Dir::deplete(path);
            Dir::remove(path);
        }
        else
            File::unlink(path);
    }
}

String File::locate(const String &fileName, const List<String> &dirs, FileAccess accessFlags)
{
    String path;
    for (const String &dir: dirs) {
        String candidate = dir / fileName;
        if (File::access(candidate, accessFlags)) {
            path = candidate;
            break;
        }
    }
    return path;
}

String File::load(const String &path)
{
    File::establish(path);
    return File{path}.readAll();
}

void File::save(const String &path, const String &text)
{
    File::establish(path);
    File{path, FileOpen::WriteOnly|FileOpen::Truncate}.write(text);
}

void File::setTimes(const String &path, double lastAccess, double lastModified, bool followSymlink)
{
    struct timespec tv[2];
    double sec;
    tv[0].tv_nsec = std::modf(lastAccess, &sec) * 1e9;
    tv[0].tv_sec = sec;
    tv[1].tv_nsec = std::modf(lastModified, &sec) * 1e9;
    tv[1].tv_sec = sec;

    if (::utimensat(AT_FDCWD, path, tv, AT_SYMLINK_NOFOLLOW * (!followSymlink)) == -1) {
        CC_SYSTEM_ERROR(errno, path);
    }
}

File::File(const String &path, FileOpen flags, FileMode mode):
    File{new File::State{::open(path, +flags|O_CLOEXEC, +mode), path, flags}}
{
    if (fd() == -1) CC_SYSTEM_ERROR(errno, path);
}

void File::truncate(off_t length)
{
    if (::ftruncate(fd(), length) == -1)
        CC_SYSTEM_ERROR(errno, path());
}

off_t File::seek(off_t distance, Seek whence)
{
    off_t ret = ::lseek(fd(), distance, +whence);
    if (ret == -1) CC_SYSTEM_ERROR(errno, path());
    return ret;
}

bool File::isSeekable() const
{
    return ::lseek(fd(), 0, SEEK_CUR) != -1;
}

off_t File::currentOffset() const
{
    off_t ret = ::lseek(fd(), 0, SEEK_CUR);
    if (ret == -1) CC_SYSTEM_ERROR(errno, path());
    return ret;
}

void File::sync()
{
    if (::fsync(fd()) == -1)
        CC_SYSTEM_ERROR(errno, path());
}

void File::dataSync()
{
#if _POSIX_SYNCHRONIZED_IO > 0
    if (::fdatasync(fd()) == -1)
#else
    if (::fsync(fd()) == -1)
#endif
        CC_SYSTEM_ERROR(errno, path());
}

String File::readAll()
{
    off_t size = ::lseek(fd(), 0, SEEK_END);
    if (size == -1) return IoStream::readAll();

    if (::lseek(fd(), 0, SEEK_SET) == -1)
        CC_SYSTEM_ERROR(errno, path());

    String buffer = String::allocate(size);
    read(&buffer);
    return buffer;
}

String File::map()
{
    off_t len = ::lseek(fd(), 0, SEEK_END);
    if (len == -1)
        CC_SYSTEM_ERROR(errno, path());

    return map(0, len);
}

String File::map(off_t i0, off_t i1)
{
    assert(0 <= i0 && i0 <= i1);
    if (i0 == i1) return String{};

    off_t len = i1 - i0;
    bool writeable = +me().flags_ & (O_WRONLY|O_RDWR);

    void *p = ::mmap(
        nullptr,
        len,
        (writeable ? PROT_WRITE : 0) | PROT_READ,
        writeable ? MAP_SHARED : MAP_PRIVATE,
        fd(),
        i0
    );

    if (p == MAP_FAILED)
        CC_SYSTEM_DEBUG_ERROR(errno);

    return String{static_cast<char *>(p), static_cast<long>(len), 1};
}

void File::close()
{
    if (me().fd_ >= 0) {
        if (::close(me().fd_) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
        me().fd_ = -1;
    }
}

} // namespace cc
