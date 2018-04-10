/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/FtFontManager>
#include <cc/ui/PlatformManager>
#include <cc/ui/PlatformPlugin>

namespace cc {
namespace ui {

PlatformPlugin *PlatformPlugin::instance()
{
    return PlatformManager::instance()->activePlugin();
}

PlatformPlugin::PlatformPlugin(String name, bool isPlatformDefault):
    name_(name),
    isPlatformDefault_(true)
{
    PlatformManager::instance()->registerPlugin(this);
}

FontManager *PlatformPlugin::fontManager() const
{
    return FtFontManager::instance();
}

}} // namespace cc::ui
