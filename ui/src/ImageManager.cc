/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/Map>
#include <cc/ValueSource>
#include <cc/ui/ImageManager>

namespace cc {
namespace ui {

ImageManager *ImageManager::instance()
{
    return Singleton<ImageManager>::instance();
}

ImageManager::ImageManager():
    plugins_{Plugins::create()}
{}

void ImageManager::registerPlugin(ImagePlugin *plugin)
{
    plugins_->establish(plugin->name(), plugin);
}

bool ImageManager::getPlugin(const string &name, const ImagePlugin **plugin) const
{
    return plugins_->lookup(name, plugin);
}

Ref< Source<const ImagePlugin *> > ImageManager::getAllPlugins() const
{
    return ValueSource<Plugins, const ImagePlugin *>::open(plugins_);
}

Ref<Image> ImageManager::load(const string &uri, unsigned index) const
{
    int j = uri->scan(':');
    if (j == 0) return Image::create();
    string scheme = uri->select(j);
    string path;
    if (j < uri->count()) path = uri->select(j + 1, uri->count());
    const ImagePlugin *plugin = 0;
    if (!getPlugin(scheme, &plugin)) return Image::create();
    return plugin->load(path, index);
}

}} // namespace cc::ui
