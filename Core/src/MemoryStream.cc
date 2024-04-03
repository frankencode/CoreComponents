/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/MemoryStream>

namespace cc {

struct MemoryStream::State: public Stream::State
{
    State(const Bytes &buffer):
        buffer_{buffer}
    {}

    long read(Out<Bytes> data, long maxFill = -1) override
    {
        if (i_ == buffer_.count()) return 0;

        if (maxFill < 0) maxFill = data->count();
        if (maxFill == 0) return 0;

        const long n = (i_ + maxFill <= buffer_.count()) ? maxFill : buffer_.count() - i_;

        buffer_.copyRangeTo(i_, i_ + n, &data);

        i_ += n;
        return n;
    }

    void write(const Bytes &data, long fill = -1) override
    {
        if (i_ == buffer_.count()) return;

        if (fill < 0) fill = data.count();
        if (fill == 0) return;

        const long n = (i_ + fill <= buffer_.count()) ? fill : buffer_.count() - i_;

        data.copyRangeToOffset(0, n, &buffer_, i_);

        i_ += n;
    }

    Bytes buffer_;
    long i_ { 0 };
};

MemoryStream::MemoryStream(const Bytes &buffer):
    Stream{new State{buffer}}
{}

} // namespace cc
