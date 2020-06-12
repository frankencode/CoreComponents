/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/UnlinkGuard>
#include <cc/File>
#include <cc/Process>

namespace cc {

UnlinkGuard::UnlinkGuard(const String &path):
    path_{path}
{
    if (path_->isRelativePath())
        path_ = path_->absolutePathRelativeTo(Process::getWorkingDirectory());
}

UnlinkGuard::~UnlinkGuard()
{
    try { File::unlink(path_); } catch (...) {}
}

} // namespace cc
