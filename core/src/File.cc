/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/mman.h> // mmap
#include <errno.h>
#include <string.h>
#include <stdio.h> // rename
#include <cc/exceptions>
#include <cc/Random>
#include <cc/Format>
#include <cc/Process>
#include <cc/Dir>
#include <cc/System>
#include <cc/File>

namespace cc {

Ref<File> File::open(const String &path, OpenMode openMode, FileMode fileMode)
{
    int fd = ::open(path, static_cast<int>(openMode), static_cast<int>(fileMode));
    if (fd == -1) CC_SYSTEM_ERROR(errno, path);
    return new File(path, openMode, fd);
}

Ref<File> File::tryOpen(const String &path, OpenMode openMode, FileMode fileMode)
{
    int fd = ::open(path, static_cast<int>(openMode), static_cast<int>(fileMode));
    if (fd != -1) return new File(path, openMode, fd);
    return 0;
}

Ref<File> File::openTemp(OpenMode openMode)
{
    String path = createUnique(
        Format("/tmp/%%_XXXXXXXX")
            << Process::exePath()->fileName()
    );
    return open(path, openMode);
}

File::File(const String &path, OpenMode openMode, int fd):
    SystemStream(fd),
    path_(path),
    openMode_(openMode)
{}

String File::path() const
{
    return path_;
}

OpenMode File::openMode() const
{
    return openMode_;
}

void File::truncate(off_t length)
{
    if (::ftruncate(fd_, length) == -1)
        CC_SYSTEM_ERROR(errno, path_);
}

off_t File::seek(off_t distance, Seek method)
{
    off_t ret = ::lseek(fd_, distance, int(method));
    if (ret == -1) CC_SYSTEM_ERROR(errno, path_);
    return ret;
}

bool File::isSeekable() const
{
    return ::lseek(fd_, 0, SEEK_CUR) != -1;
}

off_t File::transferSpanTo(off_t count, Stream *sink, CharArray *buf)
{
    if (count == 0) return 0;
    if (!sink) {
        off_t ret = 0;
        if (count > 0) ret = ::lseek(fd_, count, SEEK_CUR);
        else ret = ::lseek(fd_, 0, SEEK_END);
        if (ret != -1) return count;
    }
    return Stream::transferSpanTo(count, sink, buf);
}

class MappedByteArray: public CharArray
{
private:
    friend class File;
    MappedByteArray(char *data, int size):
        CharArray(data, size, File::unmap)
    {}
};

String File::map() const
{
    off_t fileEnd = ::lseek(fd_, 0, SEEK_END);
    if (fileEnd == -1)
        CC_SYSTEM_ERROR(errno, path_);
    size_t fileSize = fileEnd;
    if (fileSize == 0) return "";
    if (fileSize >= size_t(intMax)) fileSize = intMax;
    int pageSize = System::pageSize();
    size_t mapSize = fileSize;
    int protection = PROT_READ | (PROT_WRITE * (+openMode_ & (O_WRONLY|O_RDWR)));
    void *p = 0;
    if (fileSize % pageSize > 0) {
        mapSize += pageSize - fileSize % pageSize;
        p = ::mmap(0, fileSize, protection, MAP_PRIVATE | MAP_NORESERVE, fd_, 0);
        if (p == MAP_FAILED)
            CC_SYSTEM_ERROR(errno, path_);
    }
    else {
        #ifndef MAP_ANONYMOUS
        #define MAP_ANONYMOUS MAP_ANON
        #endif
	#ifndef MAP_NORESERVE
	#define MAP_NORESERVE 0
	#endif
        mapSize += pageSize;
        p = ::mmap(0, mapSize, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
        if (p == MAP_FAILED)
            CC_SYSTEM_DEBUG_ERROR(errno);
        p = ::mmap(p, fileSize, protection, MAP_PRIVATE | MAP_FIXED | MAP_NORESERVE, fd_, 0);
        if (p == MAP_FAILED)
            CC_SYSTEM_ERROR(errno, path_);
    }
    #ifdef MADV_SEQUENTIAL
    if (::madvise(p, mapSize, MADV_SEQUENTIAL) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    #endif
    return
        new MappedByteArray(
            reinterpret_cast<char *>(p),
            fileSize
        );
}

void File::unmap(CharArray *s)
{
    int pageSize = System::pageSize();
    size_t mapSize = s->count();
    if (s->count() % pageSize > 0) mapSize += pageSize - s->count() % pageSize;
    else mapSize += pageSize;
    ::munmap((void *)s->bytes(), mapSize);
}

void File::sync()
{
    if (::fsync(fd_) == -1)
        CC_SYSTEM_ERROR(errno, path_);
}

void File::dataSync()
{
#if _POSIX_SYNCHRONIZED_IO > 0
    if (::fdatasync(fd_) == -1)
        CC_SYSTEM_ERROR(errno, path_);
#else
    sync();
#endif
}

bool File::access(const String &path, Access flags)
{
    return ::access(path, +flags) == 0;
}

void File::create(const String &path, int mode)
{
    int fd = ::open(path, O_RDONLY|O_CREAT|O_EXCL, mode);
    if (fd == -1) CC_SYSTEM_RESOURCE_ERROR(errno, path);
    ::close(fd);
}

void File::chown(const String &path, uid_t ownerId, gid_t groupId)
{
    if (::chown(path, ownerId, groupId) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

void File::rename(const String &path, const String &newPath)
{
    if (::rename(path, newPath) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

void File::link(const String &path, const String &newPath)
{
    if (::link(path, newPath) == -1)
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
    String buf(128);
    while (true) {
        ssize_t numBytes = ::readlink(path, mutate(buf)->chars(), buf->count());
        if (numBytes == -1)
            return String();
        if (numBytes <= buf->count()) {
            if (numBytes < buf->count())
                buf = String(buf->chars(), numBytes);
            break;
        }
        buf = String(numBytes);
    }
    return buf;
}

String File::resolve(const String &path)
{
    String resolvedPath = path;
    while (FileStatus::readHead(resolvedPath)->type() == FileType::Symlink) {
        String origPath = resolvedPath;
        resolvedPath = File::readlink(resolvedPath);
        if (resolvedPath == "") break;
        if (resolvedPath->isRelativePath())
            resolvedPath = origPath->reducePath()->extendPath(resolvedPath);
    }
    return resolvedPath;
}

String File::createUnique(const String &path, int mode, char placeHolder)
{
    Ref<Random> random = Random::open(Process::currentId());
    while (true) {
        String candidate = path->copy();
        for (int i = 0, n = candidate->count(); i < n; ++i) {
            if (candidate->at(i) == placeHolder) {
                char r = random->get(0, 61);
                if ((0 <= r) && (r <= 9))
                    r += '0';
                else if ((10 <= r) && (r <= 35))
                    r += 'a' - 10;
                else if ((36 <= r) && (r <= 61))
                    r += 'A' - 36;
                mutate(candidate)->at(i) = r;
            }
        }
        int fd = ::open(candidate, O_RDONLY|O_CREAT|O_EXCL, mode);
        if (fd == -1) {
            if (errno != EEXIST)
                CC_SYSTEM_RESOURCE_ERROR(errno, candidate);
        }
        else {
            ::close(fd);
            return candidate;
        }
    }
}

void File::establish(const String &path, int fileMode, int dirMode)
{
    if (path->contains('/'))
        Dir::establish(path->reducePath(), dirMode);
    if (!File::exists(path))
        File::create(path, fileMode);
}

void File::clean(const String &path)
{
    auto status = FileStatus::read(path);
    if (status->isValid()) {
        if (status->type() == FileType::Directory) {
            Dir::deplete(path);
            Dir::remove(path);
        }
        else
            File::unlink(path);
    }
}

String File::locate(const String &fileName, const StringList *dirs, Access accessFlags)
{
    String path;
    for (int i = 0; i < dirs->count(); ++i) {
        String candidate = Format() << dirs->at(i) << "/" << fileName;
        if (access(candidate, accessFlags)) {
            path = candidate;
            break;
        }
    }
    return path;
}

String File::load(const String &path)
{
    establish(path);
    return open(path)->readAll();
}

void File::save(const String &path, const String &text)
{
    establish(path);
    Ref<File> file = open(path, OpenMode::WriteOnly);
    file->truncate(0);
    file->write(text);
}

} // namespace
