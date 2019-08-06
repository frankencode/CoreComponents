/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/File>
#include <cc/Process>
#include <cc/UnlinkGuard>

namespace cc {

UnlinkGuard::UnlinkGuard(String path):
    path_(path)
{
    if (path_->isRelativePath())
        path_ = path_->absolutePathRelativeTo(Process::getWorkingDirectory());
}

UnlinkGuard::~UnlinkGuard()
{
    try { File::unlink(path_); } catch (...) {}
}

} // namespace cc
