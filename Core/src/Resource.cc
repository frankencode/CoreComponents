/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/Resource>
#include <cc/ResourceManager>

namespace cc {

ResourceRegistration::ResourceRegistration(const String &realPrefix)
{
    ResourceManager{}.addDirectory(realPrefix);
}

} // namespace cc
