/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanFeed>
#include <cc/System>

namespace cc {
namespace can {

bool CanFeed::Instance::waitFrame(int timeout)
{
    if (timeout <= 0) return frameChannel_->wait();
    return frameChannel_->waitUntil(System::now() + 0.001 * timeout);
}

bool CanFeed::Instance::readFrame(CanFrame *frame)
{
    return frameChannel_->read(frame);
}

void CanFeed::Instance::feedFrame(const CanFrame &frame)
{
    frameChannel_->write(frame);
}

void CanFeed::Instance::shutdown()
{
    frameChannel_->close();
}

}} // namespace cc::can
