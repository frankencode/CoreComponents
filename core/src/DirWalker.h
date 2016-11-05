/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Source>
#include <cc/String>

namespace cc {

class Dir;

/** \class DirWalker DirWalker.h cc/DirWalker
  * \brief Recursive directory tree walker
  * \see Dir
  */
class DirWalker: public Source<String>
{
public:
    static Ref<DirWalker> open(String path);
    static Ref<DirWalker> tryOpen(String path);

    inline int maxDepth() const { return maxDepth_; }
    inline void setMaxDepth(int depth) { maxDepth_ = depth; }

    inline bool ignoreHidden() const { return ignoreHidden_; }
    inline void setIgnoreHidden(bool on) { ignoreHidden_ = on; }

    inline bool followSymlink() const { return followSymlink_; }
    inline void setFollowSymlink(bool on) { followSymlink_ = on; }

    inline bool deleteOrder() const { return deleteOrder_; }
    inline void setDeleteOrder(bool on) { deleteOrder_ = on; }

    bool read(String *path, bool *isDir);
    bool read(String *path) { return read(path, 0); }

private:
    DirWalker(String path, Dir *dir = 0);
    int maxDepth_;
    bool ignoreHidden_;
    bool followSymlink_;
    bool deleteOrder_;
    int depth_;
    Ref<Dir> dir_;
    Ref<DirWalker> child_;
};

CC_DEFINE_SOURCE_BEGIN_END(DirWalker);

} // namespace cc
