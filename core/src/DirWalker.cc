/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/DirWalker>
#include <cc/File>

namespace cc {

DirWalker::Instance::Instance(const String &path, const Dir &dir):
    maxDepth_{-1},
    ignoreHidden_{false},
    followSymlink_{false},
    deleteOrder_{false},
    depth_{0},
    dir_{dir}
{}

DirWalker::Instance::Instance(const String &path):
    Instance{path, Dir{path}}
{}

bool DirWalker::Instance::read(String *path, bool *isDir)
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
        try { child_ = new Instance{h}; }
        catch (...) { child_ = nullptr; }
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
