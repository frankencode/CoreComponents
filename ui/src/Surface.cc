/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Surface>
#include <cairo/cairo.h>

namespace cc {
namespace ui {

bool SurfaceInstance::isValid() const
{
    return cairo_surface_status(cairoSurface()) == CAIRO_STATUS_SUCCESS;
}

void SurfaceInstance::nextPage(bool clear)
{
    if (clear)
        cairo_surface_show_page(cairoSurface());
    else
        cairo_surface_copy_page(cairoSurface());
}

}} // namespace cc::ui
