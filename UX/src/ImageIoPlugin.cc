/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ImageIoPlugin>
#include <cc/ImageIoPluginManager>

namespace cc {

ImageIoPlugin ImageIoPlugin::detect(const String &path, const Bytes &data, Out<int> width, Out<int> height)
{
    return ImageIoPluginManager{}.detect(path, data, &width, &height);
}

ImageIoPlugin::ImageIoPlugin(State *newState):
    Object{newState}
{
    ImageIoPluginManager{}.registerPlugin(*this);
}

} // namespace cc
