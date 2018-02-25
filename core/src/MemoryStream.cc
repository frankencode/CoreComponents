/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/MemoryStream>

namespace cc {

Ref<MemoryStream> MemoryStream::open(ByteArray *buffer)
{
    return new MemoryStream(buffer);
}

MemoryStream::MemoryStream(ByteArray *buffer):
    buffer_(buffer),
    i_(0)
{}

int MemoryStream::read(ByteArray *data)
{
    if (i_ == buffer_->count()) return 0;
    const int n =
        (i_ + data->count() <= buffer_->count()) ? data->count() : buffer_->count() - i_;
    data->write(buffer_->select(i_, i_ + n));
    i_ += n;
    return n;
}

void MemoryStream::write(const ByteArray *data)
{
    if (i_ == buffer_->count()) return;
    const int n =
        (i_ + data->count() <= buffer_->count()) ? data->count() : buffer_->count() - i_;
    mutate(buffer_->select(i_, i_ + n))->write(data);
    i_ += n;
}

} // namespace cc
