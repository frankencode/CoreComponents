/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ImageIoPluginManager>

namespace cc {

void ImageIoPluginManager::State::registerPlugin(const ImageIoPlugin &plugin)
{
    pluginByName_.insert(plugin.name(), plugin);

    for (const String &ext: plugin.fileNameExtensions()) {
        pluginByExtension_.insert(ext, plugin);
    }
}

ImageIoPluginManager::ImageIoPluginManager():
    Singleton{instance<State>()}
{}

} // namespace cc
