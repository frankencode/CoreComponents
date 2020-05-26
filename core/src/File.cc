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

Ref<File> File::open(const string &path, FileOpen flags, FileMode mode)
{
    int fd = ::open(path, +flags|O_CLOEXEC, +mode);
    if (fd == -1) CC_SYSTEM_ERROR(errno, path);
    return new File{path, flags, fd};
}

Ref<File> File::tryOpen(const string &path, FileOpen flags, FileMode mode)
{
    int fd = ::open(path, +flags|O_CLOEXEC, +mode);
    if (fd != -1) return new File{path, flags, fd};
    return nullptr;
}

Ref<File> File::openTemp(FileOpen flags)
{
    string path = createUnique(
        Format{"/tmp/%%_XXXXXXXX"}
            << Process::exePath()->fileName()
    );
    return open(path, flags);
}

File::File(const string &path, FileOpen openMode, int fd):
    SystemStream{fd},
    path_{path},
    openFlags_{openMode}
{}

string File::path() const
{
    return path_;
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

string File::map() const
{
    off_t fileEnd = ::lseek(fd_, 0, SEEK_END);
    if (fileEnd == -1)
        CC_SYSTEM_ERROR(errno, path_);
    size_t fileSize = fileEnd;
    if (fileSize == 0) return "";
    if (fileSize >= size_t(intMax)) fileSize = intMax;
    int pageSize = System::pageSize();
    size_t mapSize = fileSize;
    int protection = PROT_READ | (PROT_WRITE * (+openFlags_ & (O_WRONLY|O_RDWR)));
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

bool File::checkAccess(const string &path, FileAccess flags)
{
    return ::access(path, +flags) == 0;
}

void File::create(const string &path, int mode)
{
    int fd = ::open(path, O_RDONLY|O_CREAT|O_EXCL, mode);
    if (fd == -1) CC_SYSTEM_RESOURCE_ERROR(errno, path);
    ::close(fd);
}

void File::chown(const string &path, uid_t ownerId, gid_t groupId)
{
    if (::chown(path, ownerId, groupId) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

void File::rename(const string &path, const string &newPath)
{
    if (::rename(path, newPath) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

void File::link(const string &path, const string &newPath)
{
    if (::link(path, newPath) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, newPath);
}

void File::unlink(const string &path)
{
    if (::unlink(path) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

void File::symlink(const string &path, const string &newPath)
{
    if (::symlink(path, newPath) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

string File::readlink(const string &path)
{
    string buf{128};
    while (true) {
        ssize_t numBytes = ::readlink(path, mutate(buf)->chars(), buf->count());
        if (numBytes == -1)
            return string{};
        if (numBytes <= buf->count()) {
            if (numBytes < buf->count())
                buf = string{buf->chars(), int(numBytes)};
            break;
        }
        buf = string{int(numBytes)};
    }
    return buf;
}

string File::resolve(const string &path)
{
    string resolvedPath = path;
    while (FileStatus::readHead(resolvedPath)->type() == FileType::Symlink) {
        string origPath = resolvedPath;
        resolvedPath = File::readlink(resolvedPath);
        if (resolvedPath == "") break;
        if (resolvedPath->isRelativePath())
            resolvedPath = origPath->reducePath()->extendPath(resolvedPath);
    }
    return resolvedPath;
}

string File::createUnique(const string &path, int mode, char placeHolder)
{
    Ref<Random> random = Random::open(Process::getId());
    while (true) {
        string candidate = path->copy();
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

void File::establish(const string &path, int fileMode, int dirMode)
{
    if (path->contains('/'))
        Dir::establish(path->reducePath(), dirMode);
    if (!File::exists(path))
        File::create(path, fileMode);
}

void File::clean(const string &path)
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

string File::locate(const string &fileName, const StringList *dirs, FileAccess accessFlags)
{
    string path;
    for (int i = 0; i < dirs->count(); ++i) {
        string candidate = Format{} << dirs->at(i) << "/" << fileName;
        if (checkAccess(candidate, accessFlags)) {
            path = candidate;
            break;
        }
    }
    return path;
}

string File::load(const string &path)
{
    establish(path);
    return open(path)->readAll();
}

void File::save(const string &path, const string &text)
{
    establish(path);
    Ref<File> file = open(path, FileOpen::WriteOnly);
    file->truncate(0);
    file->write(text);
}

} // namespace
