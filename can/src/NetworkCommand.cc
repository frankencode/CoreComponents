/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/NetworkCommand>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> NetworkCommand::createFrame(NetworkCommand::Specifier commandSpecifier, int targetId)
{
    CC_ASSERT(0 <= targetId && targetId <= 0x7F);
    auto frame = CanFrame::create();
    frame->setCanId(0);
    frame->setPayloadCount(2);
    frame->payloadAt(0) = static_cast<uint8_t>(commandSpecifier);
    frame->payloadAt(1) = static_cast<uint8_t>(targetId);
    return frame;
}

String NetworkCommand::Instance::toString() const
{
    if (!isValid()) return String{};

    Format f;
    f << "NetworkCommand {" << nl
      << "  specifier: " << specifier() << nl;
    if (targetId() != 0)
        f << "  targetId : " << targetId() << nl;
    f << "}";
    return f;
}

String str(NetworkCommand::Specifier commandSpecifier)
{
    String s;

    switch (commandSpecifier) {
        case NetworkCommand::Specifier::Start : s = "NetworkCommand::Specifier::Start"; break;
        case NetworkCommand::Specifier::Stop  : s = "NetworkCommand::Specifier::Stop"; break;
        case NetworkCommand::Specifier::Halt  : s = "NetworkCommand::Specifier::Halt"; break;
        case NetworkCommand::Specifier::Reboot: s = "NetworkCommand::Specifier::Reboot"; break;
        case NetworkCommand::Specifier::Reset : s = "NetworkCommand::Specifier::Reset"; break;
    };

    return s;
}

}} // namespace cc::can
