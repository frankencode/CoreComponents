/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Surface>
#include <cairo/cairo.h>

namespace cc {

void Surface::State::polish()
{}

#ifndef NDEBUG
Surface::State *Surface::paintTarget(Surface::State *newTarget)
{
    static State *target_ = nullptr;
    State *targetSaved = target_;
    target_ = newTarget;
    return targetSaved;
}
#endif

void Surface::nextPage(bool clear)
{
    if (clear)
        cairo_surface_show_page(me().cairoSurface());
    else
        cairo_surface_copy_page(me().cairoSurface());
}

} // namespace cc
