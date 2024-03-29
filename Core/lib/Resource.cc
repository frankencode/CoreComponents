/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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
