/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/NullStream>

namespace cc {

struct NullStream::State: public Stream::State
{
    long read(Out<Bytes> buffer, long maxFill) override
    {
        buffer().fill(0);
        return buffer().count();
    }

    bool isDiscarding() const override
    {
        return true;
    }
};

NullStream::NullStream():
    Stream{new State}
{}

} // namespace cc
