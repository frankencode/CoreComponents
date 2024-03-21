/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/RsvgPlugin>
#include <cc/Registration>
#include <librsvg/rsvg.h>
#include <cmath>

namespace cc {

struct RsvgPlugin::State final: public ImageIoPlugin::State
{
    State():
        ImageIoPlugin::State{"image/svg", {"svg"}}
    {}

    bool detect(const Bytes &data, Out<int> width, Out<int> height) const override
    {
        bool ok = false;
        GInputStream *stream = g_memory_input_stream_new_from_data(data, data.count(), /* destroy = */ NULL);
        RsvgHandle *handle = rsvg_handle_new_from_stream_sync(
            stream,
            /* base_file = */ NULL,
            RSVG_HANDLE_FLAG_UNLIMITED,
            /* cancellable = */ NULL,
            /* error = */ NULL
        );

        if (handle) {
            gdouble w = 0;
            gdouble h = 0;
            if (rsvg_handle_get_intrinsic_size_in_pixels(handle, &w, &h)) {
                width = std::ceil(w);
                height = std::ceil(h);
                ok = true;
            }
        }

        g_object_unref(handle);
        g_object_unref(stream);
        return ok;
    }

    Image decode(const Bytes &data) const override
    {
        Image image;

        GInputStream *stream = g_memory_input_stream_new_from_data(data, data.count(), /* destroy = */ NULL);
        RsvgHandle *handle = rsvg_handle_new_from_stream_sync(
            stream,
            /* base_file = */ NULL,
            RSVG_HANDLE_FLAG_UNLIMITED,
            /* cancellable = */ NULL,
            /* error = */ NULL
        );

        if (handle) {
            gdouble w = 0;
            gdouble h = 0;
            if (rsvg_handle_get_intrinsic_size_in_pixels(handle, &w, &h)) {
                w = std::ceil(w);
                h = std::ceil(h);
                image = Image { static_cast<int>(w), static_cast<int>(h) };
                image.clear(Color::Transparent);
                cairo_t *cr = cairo_create(image.cairoSurface());
                RsvgRectangle viewport { 0, 0, w, h };
                if (!rsvg_handle_render_document(handle, cr, &viewport, /* error = */ NULL)) {
                    image = Image{};
                }
                cairo_destroy(cr);
            }
        }

        g_object_unref(handle);
        g_object_unref(stream);

        return image;
    }

    bool decodeInto(InOut<Image> image, const Bytes &data) const override
    {
        bool ok = false;

        GInputStream *stream = g_memory_input_stream_new_from_data(data, data.count(), /* destroy = */ NULL);
        RsvgHandle *handle = rsvg_handle_new_from_stream_sync(
            stream,
            /* base_file = */ NULL,
            RSVG_HANDLE_FLAG_UNLIMITED,
            /* cancellable = */ NULL,
            /* error = */ NULL
        );

        if (handle) {
            cairo_t *cr = cairo_create(image->cairoSurface());
            RsvgRectangle viewport { 0, 0, static_cast<gdouble>(image->width()), static_cast<gdouble>(image->height()) };
            ok = rsvg_handle_render_document(handle, cr, &viewport, /* error = */ NULL);
            cairo_destroy(cr);
        }

        g_object_unref(handle);
        g_object_unref(stream);

        return ok;
    }

    void encode(const Stream &sink, const Image &image) const override
    {
        // TODO...
    }
};

RsvgPlugin::RsvgPlugin():
    ImageIoPlugin{new State}
{}

CC_REGISTRATION(RsvgPlugin)

} // namespace cc
