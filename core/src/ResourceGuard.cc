/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ResourceContext>
#include <cc/ResourceGuard>

namespace cc {

ResourceGuard::ResourceGuard(String resource)
{
    ResourceContext::instance()->push(resource);
}

ResourceGuard::~ResourceGuard()
{
    ResourceContext::instance()->pop();
}

} // namespace cc
