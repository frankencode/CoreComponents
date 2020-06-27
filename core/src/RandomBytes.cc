/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/RandomBytes>

namespace cc {

RandomBytes::Instance::Instance(int seed, off_t eoi, int byteMin, int byteMax):
    random_{seed},
    byteMin_{byteMin},
    byteMax_{byteMax},
    n_{0},
    eoi_{eoi}
{}

int RandomBytes::Instance::read(CharArray *data)
{
    if (n_ == eoi_) return 0;
    for (int i = 0; i < data->count(); ++i) {
        data->at(i) = random_->get(byteMin_, byteMax_);
        if (++n_ == eoi_) return i + 1;
    }
    return data->count();
}

} // namespace cc
