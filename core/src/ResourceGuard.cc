/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ResourceGuard>
#include <cc/ResourceContext>

namespace cc {

ResourceGuard::ResourceGuard(const String &resource)
{
    ResourceContext::instance()->push(resource);
}

ResourceGuard::~ResourceGuard()
{
    ResourceContext::instance()->pop();
}

} // namespace cc
