/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/File>
#include <cc/CleanupGuard>

namespace cc {

Ref<CleanupGuard> CleanupGuard::post(const String &path)
{
    return new CleanupGuard{path};
}

CleanupGuard::CleanupGuard(const String &path):
    path_{path}
{}

CleanupGuard::~CleanupGuard()
{
    File::clean(path_);
}

} // namespace cc
