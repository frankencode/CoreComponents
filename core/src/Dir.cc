/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/stat.h> // mkdir
#ifndef NDEBUG
#include <cc/check>
#endif
#include <cc/exceptions>
#include <cc/stdio> // DEBUG
#include <cc/File>
#include <cc/FileStatus>
#include <cc/Random>
#include <cc/Format>
#include <cc/Process>
#include <cc/DirWalker>
#include <cc/Dir>

namespace cc {

Ref<Dir> Dir::tryOpen(String path)
{
    DIR *dir = ::opendir(path);
    if (dir) return new Dir(path, dir);
    return 0;
}

Ref<Dir> Dir::openTemp()
{
    String path = createUnique(
        Format("/tmp/%%_XXXXXXXX")
            << Process::exePath()->fileName()
    );
    return open(path);
}

Dir::Dir(String path, DIR *dir):
    path_(path),
    dir_(dir)
{
    if (!dir_) {
        dir_ = ::opendir(path_);
        if (!dir_) CC_SYSTEM_RESOURCE_ERROR(errno, path);
    }
}

Dir::~Dir()
{
    #ifndef NDEBUG
    int ret =
    #endif
    ::closedir(dir_);
    #ifndef NDEBUG
    check(ret != -1);
    #endif
}

String Dir::path() const { return path_; }

bool Dir::read(String *name)
{
    struct dirent buf;
    struct dirent *result;
    while (true) {
        memclr(&buf, sizeof(buf));
        int ret = ::readdir_r(dir_, &buf, &result);
        if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
        if (result) {
            if (strcmp(buf.d_name, ".") == 0) continue;
            if (strcmp(buf.d_name, "..") == 0) continue;
            *name = buf.d_name;
        }
        break;
    }
    return result;
}

Ref<Stream> Dir::openFile(String path)
{
    if (path->isRelativePath()) path = path_->expandPath(path);
    return File::open(path);
}

bool Dir::access(String path, int flags)
{
    return ::access(path, flags) && (FileStatus::read(path)->type() == FileType::Directory);
}

bool Dir::exists(String path)
{
    return File::exists(path) && (FileStatus::read(path)->type() == FileType::Directory);
}

int Dir::count(String path)
{
    Ref<Dir> dir = tryOpen(path);
    if (!dir) return 0;
    int n = 0;
    for (String name; dir->read(&name);) {
        if (name != "." && name != "..")
            ++n;
    }
    return n;
}

void Dir::create(String path, int mode)
{
    if (::mkdir(path, mode) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

void Dir::establish(String path, int mode)
{
    Ref<StringList> missingDirs = StringList::create();
    while ((path != "") && (path != "/")) {
        if (Dir::exists(path)) break;
        missingDirs->pushFront(path);
        path = path->reducePath();
    }
    while (missingDirs->count() > 0)
        Dir::create(missingDirs->popFront(), mode);
}

void Dir::remove(String path)
{
    if (::rmdir(path) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

String Dir::createUnique(String path, int mode, char placeHolder)
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
        if (::mkdir(candidate, mode) == -1) {
            if (errno != EEXIST)
                CC_SYSTEM_RESOURCE_ERROR(errno, candidate);
        }
        else {
            return candidate;
        }
    }
}

void Dir::deplete(String path)
{
    Ref<DirWalker> walker = DirWalker::open(path);
    walker->setIgnoreHidden(false);
    walker->setFollowSymlink(false);
    walker->setDeleteOrder(true);

    String childPath;
    bool isDir = false;
    while (walker->read(&childPath, &isDir)) {
        if (isDir) Dir::remove(childPath);
        else File::unlink(childPath);
    }
}

} // namespace cc
