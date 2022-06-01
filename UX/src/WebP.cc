/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/WebP>
#include <cc/binary>
#include <cc/DEBUG>
#include <webp/decode.h>
#include <cassert>

namespace cc {

bool WebP::detect(const Bytes &data, Out<int> width, Out<int> height)
{
    return WebPGetInfo(data.bytes(), data.size(), &width, &height);
}

Image WebP::load(const Bytes &data)
{
    int width = 0;
    int height = 0;
    if (!detect(data, &width, &height)) return Image{};
    Image image{width, height};
    bool ok = false;
    if (localEndian() == ByteOrder::LittleEndian) {
        ok = (
            WebPDecodeBGRAInto(
                data.bytes(),
                data.size(),
                image.data().bytes(),
                image.data().size(),
                image.pitch()
            ) != nullptr
        );
    }
    else /* if (localEndian() == ByteOrder::BigEndian) */ {
        ok = (
            WebPDecodeRGBAInto(
                data.bytes(),
                data.size(),
                image.data().bytes(),
                image.data().size(),
                image.pitch()
            ) != nullptr
        );
    }
    return ok ? image : Image{};
}

bool WebP::loadInto(InOut<Image> image, const Bytes &data)
{
    if (!image()) {
        image = load(data);
        return !image->isNull();
    }

    WebPDecoderConfig config;
    if (!WebPInitDecoderConfig(&config)) {
        assert(false);
        return false;
    }

    config.options.no_fancy_upsampling = 0;
    config.options.use_scaling = 0;
    config.options.scaled_width = image->width();
    config.options.scaled_height = image->height();
    config.options.use_threads = 1;

    if (localEndian() == ByteOrder::LittleEndian)
        config.output.colorspace = MODE_BGRA;
    else /* if (localEndian() == ByteOrder::BigEndian) */
        config.output.colorspace = MODE_RGBA;

    config.output.u.RGBA.rgba = image->data().bytes();
    config.output.u.RGBA.stride = image->pitch();
    config.output.u.RGBA.size = image->data().size();
    config.output.is_external_memory = 1;

    return WebPDecode(data.bytes(), data.size(), &config) == VP8_STATUS_OK;
}

} // namespace cc
