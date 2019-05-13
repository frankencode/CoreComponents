/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cairo/cairo.h>
#include <cairo/cairo-pdf.h>
#include <cc/ui/PdfSurface>

namespace cc {
namespace ui {

Ref<PdfSurface> PdfSurface::open(Stream *stream, Size sizePt)
{
    return new PdfSurface(stream, sizePt);
}

PdfSurface::PdfSurface(Stream *stream, Size sizePt):
    stream_{stream},
    cairoSurface_(
        cairo_pdf_surface_create_for_stream(cairoWrite, this, sizePt[0], sizePt[1])
    )
{}

PdfSurface::~PdfSurface()
{
    if (cairoSurface_)
        cairo_surface_destroy(cairoSurface_);
}

cairo_surface_t *PdfSurface::cairoSurface() const
{
    return cairoSurface_;
}

cairo_status_t PdfSurface::cairoWrite(void *closure, const unsigned char *data, unsigned int length)
{
    try {
        reinterpret_cast<PdfSurface *>(closure)->stream_->write(data, length);
    }
    catch (...)
    {
        return CAIRO_STATUS_WRITE_ERROR;
    }
    return CAIRO_STATUS_SUCCESS;
}

}} // namespace cc::ui
