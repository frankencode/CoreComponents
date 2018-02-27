/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ReplaySource>

namespace cc {

ReplaySource::ReplaySource(const CharArray *buffer):
    buffer_(buffer),
    i_(0)
{}

int ReplaySource::read(CharArray *data)
{
    if (i_ == buffer_->count()) return 0;
    const int n =
        (i_ + data->count() <= buffer_->count()) ? data->count() : (buffer_->count() - i_);
    data->write(buffer_->select(i_, i_ + n));
    i_ += n;
    return n;
}

} // namespace cc
