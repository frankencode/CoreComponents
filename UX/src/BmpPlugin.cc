/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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

    Image decode(const Bytes &data) const override
    {
        return Bmp::decode(data);
    }

    bool decodeInto(InOut<Image> image, const Bytes &data) const override
    {
        if (!image()) {
            image = decode(data);
            return !image->isNull();
        }
        return false;
    }

    void encode(const Stream &sink, const Image &image) const override
    {
        Bmp::encode(sink, image);
    }
};

BmpPlugin::BmpPlugin():
    ImageIoPlugin{new State}
{}

CC_REGISTRATION(BmpPlugin)

} // namespace cc
