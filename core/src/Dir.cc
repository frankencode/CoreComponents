/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Dir>
#include <cc/DirWalker>
#include <cc/FileStatus>
#include <cc/File>
#include <cc/Random>
#include <cc/str>
#include <cstring>
#include <dirent.h> // DIR, opendir, closedir, readdir

namespace cc {

bool Dir::access(const String &path, FileAccess mode)
{
    return ::access(path, +mode) == 0 && FileStatus{path}.type() == FileType::Directory;
}

bool Dir::exists(const String &path)
{
    return FileStatus{path}.type() == FileType::Directory;
}

void Dir::create(const String &path, FileMode mode)
{
    if (::mkdir(path, +mode) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

void Dir::establish(const String &path, FileMode mode)
{
    List<String> missingDirs;
    for (
        String p = path;
        p.count() > 0 && p != "/";
        p = p.cdUp()
    ) {
        if (Dir::exists(p)) break;
        missingDirs.pushFront(p);
    }
    while (missingDirs.count() > 0) {
        const String &head = missingDirs.first();
        if (::mkdir(head, +mode) == -1) {
            if (!(errno == EEXIST && Dir::exists(head)))
                CC_SYSTEM_RESOURCE_ERROR(errno, head);
        }
        missingDirs.popFront();
    }
}

String Dir::createUnique(const String &path, FileMode mode, char placeHolder)
{
    assert(path.find(placeHolder));

    String candidate;

    for (Random random; true;) {
        candidate = path.copy();
        for (int i = 0, n = candidate.count(); i < n; ++i) {
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
        if (::mkdir(candidate, +mode) == -1) {
            if (!(errno == EEXIST && Dir::exists(candidate)))
                CC_SYSTEM_RESOURCE_ERROR(errno, candidate);
        }
        else {
            break;
        }
    }

    return candidate;
}

String Dir::createTemp(FileMode mode)
{
    return createUnique("/tmp/" + str(::getpid()) + "_########", mode, '#');
}

void Dir::remove(const String &path)
{
    if (::rmdir(path) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

void Dir::deplete(const String &path)
{
    DirWalker walker{path};
    walker.setIgnoreHidden(false);
    walker.setFollowSymlink(false);
    walker.setDeleteOrder(true);

    String childPath;
    bool isDir = false;
    while (walker.read(&childPath, &isDir)) {
        if (isDir) Dir::remove(childPath);
        else File::unlink(childPath);
    }
}

struct Dir::State: public Object::State
{
    using Default = Defined;

    String path;
    DIR *dir { nullptr };

    ~State()
    {
        if (dir) ::closedir(dir);
    }
};

Dir::Dir(const String &path):
    Object{new State}
{
    me().path = path;
    me().dir = ::opendir(path);
    if (!me().dir) CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

String Dir::path() const
{
    return me().path;
}

bool Dir::read(Out<String> name)
{
    struct dirent *entry = nullptr;

    while (true) {
        errno = 0;
        entry = ::readdir(me().dir);
        if (!entry && errno) CC_SYSTEM_DEBUG_ERROR(errno);
        if (entry) {
            if (std::strcmp(entry->d_name, ".") == 0) continue;
            if (std::strcmp(entry->d_name, "..") == 0) continue;
            name = entry->d_name;
        }
        break;
    }

    return entry;
}

Stream Dir::openFile(const String &path, FileOpen flags)
{
    String targetPath = path.isRelativePath() ? (me().path / path) : path;
    return File{targetPath, flags};
}

Dir::State &Dir::me()
{
    return Object::me.as<State>();
}

const Dir::State &Dir::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
