/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ReplaySource>

namespace cc {

struct ReplaySource::State: public Stream::State
{
    State(const Bytes &buffer):
        recording_{buffer}
    {}

    long read(Out<Bytes> buffer, long maxFill) override
    {
        long avail = recording_.count() - i0_;
        if (avail == 0) return 0;

        if (maxFill < 0) maxFill = buffer->count();
        if (maxFill == 0) return 0;

        if (maxFill > avail) maxFill = avail;

        recording_.copyRangeTo(i0_, i0_ + maxFill, &buffer);
        i0_ += maxFill;

        return maxFill;
    }

    Bytes recording_;
    long i0_ { 0 };
};

ReplaySource::ReplaySource(const Bytes &buffer):
    Stream{new ReplaySource::State{buffer}}
{}

} // namespace cc
