/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/VirtualCanBridge>

namespace cc {
namespace can {

VirtualCanBridge::Instance::Instance(const CanMedia &mediaA, const CanMedia &mediaB):
    mediaA_{mediaA},
    mediaB_{mediaB}
{
    aToB_ = Worker{[=]{
        for (CanFrame frame; mediaA_->readFrame(&frame);)
            mediaB_->writeFrame(frame);
    }};
    bToA_ = Worker{[=]{
        for (CanFrame frame; mediaB_->readFrame(&frame);)
            mediaA_->writeFrame(frame);
    }};
}

}} // namespace cc::can
