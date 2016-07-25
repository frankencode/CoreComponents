/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Dir>
#include <cc/CleanupGuard>

namespace cc {

CleanupGuard::CleanupGuard(String path):
    path_(path)
{}

CleanupGuard::~CleanupGuard()
{
    Dir::cleanup(path_);
}

} // namespace cc
