/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Dir>
#include <cc/File>
#include <cc/DirWalker>

namespace cc {

Ref<DirWalker> DirWalker::open(String path)
{
    return new DirWalker(path);
}

Ref<DirWalker> DirWalker::tryOpen(String path)
{
    Ref<Dir> dir = Dir::tryOpen(path);
    Ref<DirWalker> walker;
    if (dir) walker = new DirWalker(path, dir);
    return walker;
}

DirWalker::DirWalker(String path, Dir *dir):
    maxDepth_(-1),
    ignoreHidden_(false),
    followSymlink_(false),
    deleteOrder_(false),
    depth_(0),
    dir_(dir)
{
    if (!dir_) dir_ = Dir::open(path);
}

bool DirWalker::read(String *path, bool *isDir)
{
    if (child_) {
        if (child_->read(path, isDir))
            return true;
        if (deleteOrder_) {
            *path = child_->dir_->path();
            if (isDir) *isDir = true;
            child_ = 0;
            return true;
        }
        child_ = 0;
    }
    String name;
    while (dir_->read(&name)) {
        if (name == "") continue;
        if (ignoreHidden_) if (name->at(0) == '.') continue;
        String h = dir_->path()->extendPath(name);
        child_ = tryOpen(h);
        bool d = child_;
        if (child_) {
            if (depth_ != maxDepth_) {
                if (!followSymlink_ && File::readlink(h) != "") {
                    child_ = 0;
                }
                else {
                    child_->maxDepth_ = maxDepth_;
                    child_->followSymlink_ = followSymlink_;
                    child_->deleteOrder_ = deleteOrder_;
                    child_->depth_ = depth_ + 1;
                    if (deleteOrder_)
                        return read(path, isDir);
                }
            }
            else {
                child_ = 0;
            }
        }
        *path = h;
        if (isDir) *isDir = d;
        return true;
    }
    return false;
}

} // namespace cc
