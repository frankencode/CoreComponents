/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/WebPPlugin>
#include <cc/WebP>
#include <cc/Registration>

namespace cc {

struct WebPPlugin::State final: public ImageIoPlugin::State
{
    State():
        ImageIoPlugin::State{"image/webp", {"webp"}}
    {}

    bool detect(const Bytes &data, Out<int> width, Out<int> height) const override
    {
        return WebP::detect(data, &width, &height);
    }

    Image decode(const Bytes &data) const override
    {
        return WebP::decode(data);
    }

    bool decodeInto(InOut<Image> image, const Bytes &data) const override
    {
        return WebP::decodeInto(&image, data);
    }

    void encode(const Stream &sink, const Image &image) const override
    {
        WebP::encode(sink, image);
    }
};

WebPPlugin::WebPPlugin():
    ImageIoPlugin{new State}
{}

CC_REGISTRATION(WebPPlugin)

} // namespace cc
