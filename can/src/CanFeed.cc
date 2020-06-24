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

CanFeed::Instance::Instance()
{}

CanFeed::Instance::~Instance()
{}

bool CanFeed::Instance::waitFrame(int timeout)
{
    if (timeout <= 0) {
        frameChannel_->waitForNotEmpty();
        return true;
    }
    return frameChannel_->waitForNotEmptyUntil(System::now() + 0.001 * timeout);
}

bool CanFeed::Instance::readFrame(CanFrame *frame)
{
    return bool(*frame = frameChannel_->popFront());
}

void CanFeed::Instance::feedFrame(const CanFrame &frame)
{
    frameChannel_->pushBack(frame);
}

void CanFeed::Instance::shutdown()
{
    frameChannel_->pushBack(CanFrame{});
}

}} // namespace cc::can
