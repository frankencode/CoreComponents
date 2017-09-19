/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Application>
#include <cc/ui/sdl/FtFontProvider>
#include <cc/ui/sdl/FtActivation>

namespace cc {
namespace ui {

FtActivation::FtActivation(Font *font, FtFontFace **face)
{
    *face = Object::cast<FtFontProvider *>(Application::instance()->fontProvider())
        ->activateFace(font);
}

}} // namespace cc::ui
