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

    Image load(const Bytes &data) const override
    {
        return WebP::load(data);
    }

    bool loadInto(InOut<Image> image, const Bytes &data) const override
    {
        return WebP::loadInto(&image, data);
    }
};

WebPPlugin::WebPPlugin():
    ImageIoPlugin{new State}
{}

CC_REGISTRATION(WebPPlugin)

} // namespace cc
