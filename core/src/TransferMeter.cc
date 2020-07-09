/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TransferMeter>

namespace cc {

TransferMeter::Instance::Instance(const Stream &stream):
    stream_{stream},
    totalRead_{0},
    totalWritten_{0}
{}

int TransferMeter::Instance::read(CharArray *buf)
{
    int n = stream_->read(buf);
    totalRead_ += n;
    return n;
}

void TransferMeter::Instance::write(const CharArray *buf)
{
    stream_->write(buf);
    totalWritten_ += buf->count();
}

void TransferMeter::Instance::write(const StringList &parts)
{
    stream_->write(parts);
    for (const String &s: parts)
        totalWritten_ += s->count();
}

} // namespace cc
