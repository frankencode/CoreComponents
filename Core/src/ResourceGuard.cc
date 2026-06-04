/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
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
