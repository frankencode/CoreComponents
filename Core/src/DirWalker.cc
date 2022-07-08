/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/DirWalker>
#include <cc/Dir>
#include <cc/File>

namespace cc {

struct DirWalker::State: public Object::State
{
    State() = default;

    State(const String &path):
        dir{path}
    {}

    bool read(Out<String> path, Out<bool> isDir)
    {
        if (child) {
            if (child().read(path, isDir))
                return true;
            if (deleteOrder) {
                path << child().dir.path();
                isDir << true;
                child = nullptr;
                return true;
            }
            child = nullptr;
        }

        for (String name; dir.read(&name);)
        {
            if (name == "") continue;
            if (ignoreHidden && name.startsWith('.')) continue;

            String h = dir.path() / name;
            try { child = new State{h}; }
            catch (...) { child = nullptr; }
            bool d = child;
            if (child) {
                if (depth != maxDepth) {
                    if (!followSymlink && File::readlink(h) != "") {
                        child = nullptr;
                    }
                    else {
                        child().maxDepth = maxDepth;
                        child().followSymlink = followSymlink;
                        child().deleteOrder = deleteOrder;
                        child().depth = depth + 1;
                        if (deleteOrder)
                            return read(path, isDir);
                    }
                }
                else {
                    child = nullptr;
                }
            }
            path << h;
            isDir << d;
            return true;
        }
        return false;
    }

    Dir dir;
    int maxDepth { -1 };
    bool ignoreHidden { false };
    bool followSymlink { false };
    bool deleteOrder { false };
    int depth { 0 };
    Handle<State> child { nullptr };
};

DirWalker::DirWalker(const String &path):
    Object{new State{path}}
{}

bool DirWalker::read(Out<String> path, Out<bool> isDir)
{
    return me().read(path, isDir);
}

void DirWalker::setMaxDepth(int depth)
{
    me().maxDepth = depth;
}

void DirWalker::setIgnoreHidden(bool on)
{
    me().ignoreHidden = on;
}

void DirWalker::setFollowSymlink(bool on)
{
    me().followSymlink = on;
}

void DirWalker::setDeleteOrder(bool on)
{
    me().deleteOrder = on;
}

DirWalker::State &DirWalker::me()
{
    return Object::me.as<State>();
}

const DirWalker::State &DirWalker::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
