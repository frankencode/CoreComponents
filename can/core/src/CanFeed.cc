/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanFeed>
#include <cc/Channel>
#include <cc/System>

namespace cc {
namespace can {

CanFeed::CanFeed():
    frameChannel_{FrameChannel::create()}
{}

CanFeed::~CanFeed()
{}

bool CanFeed::waitFrame(int timeout)
{
    if (timeout <= 0) {
        frameChannel_->waitForNotEmpty();
        return true;
    }
    return frameChannel_->waitForNotEmptyUntil(System::now() + 0.001 * timeout);
}

bool CanFeed::readFrame(CanFrame *frame)
{
    auto b = frameChannel_->popFront();
    if (b) *frame = *b;
    return b;
}

void CanFeed::feedFrame(const CanFrame *frame)
{
    frameChannel_->pushBack(frame);
}

void CanFeed::shutdown()
{
    frameChannel_->pushBack(nullptr);
}

}} // namespace cc::can
