/*
 * Copyright (C) 2007-2017 Frank Mertens.
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
  * \ingroup file_system
  * \brief Recursive directory tree traversal
  * \see Dir
  */
class DirWalker: public Source<String>
{
public:
    /** Open a directory for recursive tree traversal
      * \param path directory path
      * \return new object instance
      */
    static Ref<DirWalker> open(String path);

    /** Open a directory for recursive tree traversal, if it exists
      * \param path directory path
      * \return new object instance
      */
    static Ref<DirWalker> tryOpen(String path);

    /// Maximum recursive tree traversal depth (or -1 if none)
    inline int maxDepth() const { return maxDepth_; }

    /// \copydoc maxDepth()
    inline void setMaxDepth(int depth) { maxDepth_ = depth; }

    /// Overstep hidden files and directories during recursive tree traversal
    inline bool ignoreHidden() const { return ignoreHidden_; }

    /// \copydoc ignoreHidden()
    inline void setIgnoreHidden(bool on) { ignoreHidden_ = on; }

    /// Follow symbolic links during recursive tree traversal
    inline bool followSymlink() const { return followSymlink_; }

    /// \copydoc followSymlink()
    inline void setFollowSymlink(bool on) { followSymlink_ = on; }

    /// Child-first recursive tree traversal order
    inline bool deleteOrder() const { return deleteOrder_; }

    /// \copydoc deleteOrder()
    inline void setDeleteOrder(bool on) { deleteOrder_ = on; }

    /** Read next file or directory path
      * \param path return the file or directory path
      * \param isDir true if the return path points to a directory
      * \return true if not end of information
      */
    bool read(String *path, bool *isDir);

    /** Read next file or directory path
      * \param path return the file or directory path
      * \return true if not end of information
      */
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

} // namespace cc
