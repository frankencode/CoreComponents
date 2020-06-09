/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/VirtualCanBridge>
#include <cc/Worker>

namespace cc {
namespace can {

Ref<VirtualCanBridge> VirtualCanBridge::create(CanMedia *mediaA, CanMedia *mediaB)
{
    Ref<VirtualCanBridge> bridge = new VirtualCanBridge{mediaA, mediaB};
    bridge->init();
    return bridge;
}

VirtualCanBridge::VirtualCanBridge(CanMedia *mediaA, CanMedia *mediaB):
    mediaA_{mediaA},
    mediaB_{mediaB}
{}

void VirtualCanBridge::init()
{
    aToB_ = Worker::start([=]{
        for (auto frame = CanFrame::create(); mediaA_->readFrame(frame);)
            mediaB_->writeFrame(frame);
    });
    bToA_ = Worker::start([=]{
        for (auto frame = CanFrame::create(); mediaB_->readFrame(frame);)
            mediaA_->writeFrame(frame);
    });
}

void VirtualCanBridge::log(const CanFrame *frame) const
{}

}} // namespace cc::can
