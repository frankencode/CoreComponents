/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cairo/cairo.h>
#include <cairo/cairo-svg.h>
#include <cc/ui/SvgSurface>

namespace cc {
namespace ui {

Ref<SvgSurface> SvgSurface::open(Stream *stream, Size sizePt)
{
    return new SvgSurface{stream, sizePt};
}

SvgSurface::SvgSurface(Stream *stream, Size sizePt):
    stream_{stream},
    cairoSurface_(
        cairo_svg_surface_create_for_stream(cairoWrite, this, sizePt[0], sizePt[1])
    )
{}

SvgSurface::~SvgSurface()
{
    if (cairoSurface_)
        cairo_surface_destroy(cairoSurface_);
}

cairo_surface_t *SvgSurface::cairoSurface() const
{
    return cairoSurface_;
}

cairo_status_t SvgSurface::cairoWrite(void *closure, const unsigned char *data, unsigned int length)
{
    try {
        reinterpret_cast<SvgSurface *>(closure)->stream_->write(data, length);
    }
    catch (...)
    {
        return CAIRO_STATUS_WRITE_ERROR;
    }
    return CAIRO_STATUS_SUCCESS;
}

}} // namespace cc::ui
