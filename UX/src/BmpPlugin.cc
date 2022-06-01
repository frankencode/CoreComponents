/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/BmpPlugin>
#include <cc/Bmp>
#include <cc/Registration>

namespace cc {

struct BmpPlugin::State final: public ImageIoPlugin::State
{
    State():
        ImageIoPlugin::State{"image/bmp", {"bmp"}}
    {}

    bool detect(const Bytes &data, Out<int> width, Out<int> height) const override
    {
        return Bmp::detect(data, &width, &height);
    }

    Image load(const Bytes &data) const override
    {
        return Bmp::load(data);
    }

    bool loadInto(InOut<Image> image, const Bytes &data) const override
    {
        if (!image()) {
            image = load(data);
            return !image->isNull();
        }
        return false;
    }
};

CC_REGISTRATION(BmpPlugin)

} // namespace cc
