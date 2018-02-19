/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cairo/cairo.h>
#include <cc/ui/Surface>

namespace cc {
namespace ui {

bool Surface::isValid() const
{
    return cairo_surface_status(cairoSurface()) == CAIRO_STATUS_SUCCESS;
}

void Surface::nextPage(bool clear)
{
    if (clear)
        cairo_surface_show_page(cairoSurface());
    else
        cairo_surface_copy_page(cairoSurface());
}

}} // namespace cc::ui
