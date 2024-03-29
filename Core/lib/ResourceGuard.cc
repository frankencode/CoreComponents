/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ResourceGuard>
#include <cc/ResourceContext>

namespace cc {

ResourceGuard::ResourceGuard(const String &resource)
{
    ResourceContext{}.push(resource);
}

ResourceGuard::~ResourceGuard()
{
    ResourceContext{}.pop();
}

} // namespace cc