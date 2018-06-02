/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/Map>
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

bool ImageManager::getPlugin(const String &name, const ImagePlugin **plugin) const
{
    return plugins_->lookup(name, plugin);
}

Ref< Source<const ImagePlugin *> > ImageManager::getAllPlugins() const
{
    return plugins_->getAllValues<const ImagePlugin *>();
}

Ref<Image> ImageManager::load(const String &uri, unsigned index) const
{
    int j = uri->find(':');
    if (j == 0 || j == uri->count()) return Ref<Image>{}; // throw ImageLoadingError(Format("Mailformed URI (%%)") << uri);
    String scheme = uri->select(j);
    String path = uri->select(j + 1, uri->count());
    const ImagePlugin *plugin = 0;
    if (!getPlugin(scheme, &plugin)) return Ref<Image>{}; // throw ImageLoadingError(Format("No image plugin handling scheme '%%' found (%%)") << scheme << uri);
    return plugin->load(path, index);
}

}} // namespace cc::ui
