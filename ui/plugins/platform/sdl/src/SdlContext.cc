/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/SdlContext>

namespace cc {
namespace ui {

SdlContext::~SdlContext()
{
    if (sdlTexture_) SDL_DestroyTexture(sdlTexture_);
}

}} // namespace cc::ui
