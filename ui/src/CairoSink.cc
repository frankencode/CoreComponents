/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/CairoSink>

namespace cc {
namespace ui {

Ref<CairoSink> CairoSink::open(Stream *stream)
{
    return new CairoSink{stream};
}

cairo_status_t CairoSink::cairoWrite(void *closure, const unsigned char *data, unsigned int length)
{
    try {
        reinterpret_cast<CairoSink *>(closure)->stream_->write(data, length);
    }
    catch (...) {
        return CAIRO_STATUS_WRITE_ERROR;
    }
    return CAIRO_STATUS_SUCCESS;
}

CairoSink::CairoSink(Stream *stream):
    stream_{stream}
{}

}} // namespace cc::ui
