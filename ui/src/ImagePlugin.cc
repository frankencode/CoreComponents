/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Image>
#include <cc/ui/ImageManager>
#include <cc/ui/ImagePlugin>

namespace cc {
namespace ui {

ImagePlugin::ImagePlugin(const String &name):
    name_{name}
{}

void ImagePlugin::init()
{
    ImageManager::instance()->registerPlugin(this);
}

}} // namespace cc::ui
