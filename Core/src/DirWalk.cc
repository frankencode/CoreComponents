/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/DirWalk>
#include <cc/Dir>
#include <cc/File>

namespace cc {

struct DirWalk::State: public Object::State
{
    State() = default; // FIXME: Whatfore?!

    State(const String &path, DirWalk::Mode mode, int maxDepth):
        path_{path},
        mode_{mode},
        maxDepth_{maxDepth}
    {
        try {
            dir_ = Dir{path};
        }
        catch (SystemError &error)
        {
            if (error.errorCode() == ENOTDIR) mode_ |= DirWalk::NotADir;
            else if (error.errorCode() != ENOENT) throw;
        }
    }

    State(const Dir &dir, DirWalk::Mode mode, int maxDepth):
        mode_{mode},
        maxDepth_{maxDepth},
        dir_{dir}
    {}

    bool read(Out<String> path)
    {
        if (mode_ & DirWalk::FilesOnly) {
            for (bool isDir = false; read(&path, &isDir);) {
                if (!isDir) return true;
            }
            return false;
        }
        else if (mode_ & DirWalk::DirsOnly) {
            for (bool isDir = false; read(&path, &isDir);) {
                if (isDir) return true;
            }
            return false;
        }
        bool dummy = false;
        return read(&path, &dummy);
    }

    bool read(Out<String> path, Out<bool> isDir)
    {
        if (!dir_) {
            if (mode_ & DirWalk::NotADir) {
                path << path_;
                isDir << false;
                mode_ ^= DirWalk::NotADir;
                return true;
            }
            return false;
        }

        if (child_) {
            if (child_().read(&path, &isDir)) {
                return true;
            }
            if (deleteOrder()) {
                path << child_().dir_.path();
                isDir << true;
                child_ = nullptr;
                return true;
            }
            child_ = nullptr;
        }

        for (String name; dir_.read(&name);)
        {
            if (name == "") continue;
            if (ignoreHidden() && name.startsWith('.')) continue;

            String h = name;
            if (dir_.path() != ".") h = dir_.path() / h;

            try {
                if (!followSymlink() && File::readlink(h)) child_ = nullptr;
                else child_ = new State{Dir{h}, mode_, maxDepth_};
            }
            catch (...)
            {
                child_ = nullptr;
            }
            bool d = child_;
            if (child_) {
                if (depth_ != maxDepth_) {
                    if (!followSymlink() && File::readlink(h) != "") {
                        child_ = nullptr;
                    }
                    else {
                        child_().depth_ = depth_ + 1;
                        if (deleteOrder()) return read(&path, &isDir);
                    }
                }
                else {
                    child_ = nullptr;
                }
            }
            path << h;
            isDir << d;
            return true;
        }

        return false;
    }

    bool onlyFiles() const { return mode_ & DirWalk::FilesOnly; }
    bool onlyDirs() const { return mode_ & DirWalk::DirsOnly; }
    bool deleteOrder() const { return mode_ & DirWalk::DeleteOrder; }
    bool ignoreHidden() const { return mode_ & DirWalk::IgnoreHidden; }
    bool followSymlink() const { return mode_ & DirWalk::FollowSymlink; }

    String path_;
    DirWalk::Mode mode_ { DirWalk::Default };
    int maxDepth_ { -1 };
    int depth_ { 0 };
    Dir dir_;
    Handle<State> child_ { nullptr };
};

DirWalk::DirWalk(const String &path, Mode mode, int maxDepth):
    Object{new State{path, mode, maxDepth}}
{}

bool DirWalk::read(Out<String> path)
{
    return me().read(path);
}

bool DirWalk::read(Out<String> path, Out<bool> isDir)
{
    return me().read(path, isDir);
}

DirWalk::State &DirWalk::me()
{
    return Object::me.as<State>();
}

const DirWalk::State &DirWalk::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
