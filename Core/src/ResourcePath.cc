/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/ResourcePath>
#include <cc/ResourceManager>

namespace cc {

ResourcePath::ResourcePath(const String &path):
    path_{path}
{}

ResourcePath::operator String() const
{
    return ResourceManager{}.realPath(path_);
}

} // namespace cc
