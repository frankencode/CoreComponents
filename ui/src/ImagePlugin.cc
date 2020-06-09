/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ImagePlugin>
#include <cc/ui/ImageManager>
#include <cc/ui/Image>

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
