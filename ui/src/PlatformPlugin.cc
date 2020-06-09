/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PlatformPlugin>
#include <cc/ui/FtFontManager>
#include <cc/ui/PlatformManager>

namespace cc {
namespace ui {

PlatformPlugin *PlatformPlugin::instance()
{
    return PlatformManager::instance()->activePlugin();
}

PlatformPlugin::PlatformPlugin(const String &name, bool isPlatformDefault):
    name_{name},
    isPlatformDefault_{true}
{}

void PlatformPlugin::init()
{
    PlatformManager::instance()->registerPlugin(this);
}

FontManager *PlatformPlugin::fontManager() const
{
    return FtFontManager::instance();
}

}} // namespace cc::ui
