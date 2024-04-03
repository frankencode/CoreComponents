/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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
