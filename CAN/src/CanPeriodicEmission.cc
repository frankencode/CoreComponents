/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/CanPeriodicEmission>

namespace cc {

void CanPeriodicEmission::State::updateData(const Bytes &data)
{
    Guard<Mutex> guard { mutex_ };
    const long n = data.count();
    frame_.size(n);
    for (long i = 0; i < n; ++i) {
        frame_(i, data(i));
    }
}

void CanPeriodicEmission::State::generate(CanMedia &media)
{
    Guard<Mutex> guard { mutex_ };
    media.write(frame_);
}

CanPeriodicEmission::CanPeriodicEmission(int canId, double interval, const Bytes &initialData):
    Object{new State{canId, interval}}
{
    me().updateData(initialData);
}

} // namespace cc
