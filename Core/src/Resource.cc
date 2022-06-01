/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
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
