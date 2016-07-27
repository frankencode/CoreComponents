/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/CaptureSink>

namespace cc {

CaptureSink::CaptureSink():
    parts_(StringList::create())
{}

void CaptureSink::write(const ByteArray *data)
{
    parts_->append(data);
}

Ref<ByteArray> CaptureSink::collect() const
{
    return parts_->join();
}

} // namespace cc