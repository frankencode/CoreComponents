/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ImageIoPlugin>
#include <cc/ImageIoPluginManager>

namespace cc {

ImageIoPlugin ImageIoPlugin::detect(const String &path, const Bytes &data, Out<int> width, Out<int> height)
{
    return ImageIoPluginManager{}.detect(path, data, &width, &height);
}

ImageIoPlugin::ImageIoPlugin(const String &mediaType, const String &fileSuffix)
{
    if (mediaType != "") {
        *this = ImageIoPluginManager{}.pluginByMediaType(mediaType);
    }
    else if (fileSuffix != "") {
        *this = ImageIoPluginManager{}.pluginByFileSuffix(fileSuffix);
    }
}

ImageIoPlugin::ImageIoPlugin(State *newState):
    Object{newState}
{
    ImageIoPluginManager{}.registerPlugin(*this);
}

} // namespace cc
