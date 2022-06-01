/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ImageIoPluginManager>
#include <cc/MultiMap>
#include <cc/Map>

namespace cc {

struct ImageIoPluginManager::State final: public Singleton::State
{
    void registerPlugin(const ImageIoPlugin &plugin)
    {
        pluginByMediaType_.insert(plugin.mediaType(), plugin);

        for (const String &suffix: plugin.fileSuffix()) {
            pluginBySuffix_.insert(suffix, plugin);
        }
    }

    ImageIoPlugin detect(const String &path, const Bytes &data, Out<int> width, Out<int> height) const
    {
        ImageIoPlugin plugin;
        String suffix = path.fileSuffix();
        Locator target;
        if (pluginBySuffix_.find(suffix, &target)) {
            do {
                ImageIoPlugin candidate = pluginBySuffix_.at(target).value();
                if (candidate.detect(data, &width, &height)) {
                    plugin = candidate;
                    break;
                }
                ++target;
            }
            while (target && pluginBySuffix_.at(target).key() == suffix);
        }
        return plugin;
    }

    MultiMap<String, ImageIoPlugin> pluginBySuffix_;
    Map<String, ImageIoPlugin> pluginByMediaType_;
};

ImageIoPluginManager::ImageIoPluginManager():
    Singleton{instance<State>()}
{}

ImageIoPlugin ImageIoPluginManager::pluginByMediaType(const String &mediaType) const
{
    return me().pluginByMediaType_(mediaType);
}

ImageIoPlugin ImageIoPluginManager::pluginByFileSuffix(const String &fileSuffix) const
{
    Locator target;
    if (me().pluginBySuffix_.find(fileSuffix, &target)) {
        return me().pluginBySuffix_.at(target).value();
    }
    return ImageIoPlugin{};
}

ImageIoPlugin ImageIoPluginManager::detect(const String &path, const Bytes &data, Out<int> width, Out<int> height) const
{
    return me().detect(path, data, &width, &height);
}

void ImageIoPluginManager::registerPlugin(const ImageIoPlugin &plugin)
{
    me().registerPlugin(plugin);
}

ImageIoPluginManager::State &ImageIoPluginManager::me()
{
    return Object::me.as<State>();
}

const ImageIoPluginManager::State &ImageIoPluginManager::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
