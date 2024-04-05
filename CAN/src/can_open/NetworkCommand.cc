/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/NetworkCommand>
#include <cc/Format>
#include <cassert>

namespace cc::can_open {

CanFrame NetworkCommand::createFrame(NetworkCommand::Specifier command, int targetId)
{
    assert(0 <= targetId && targetId <= 0x7F);

    return
        CanFrame { 0 }
        .size(2)
        (0, +command)
        (1, static_cast<uint8_t>(targetId));
}

String NetworkCommand::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << "\",\n"
        "  \"command\": \"" << specifier() << "\",\n"
        "  \"target\": " << targetId() << "\n"
        "}";
}

String str(NetworkCommand::Specifier commandSpecifier)
{
    String s;

    switch (commandSpecifier) {
        case NetworkCommand::Specifier::Start : s = "start"; break;
        case NetworkCommand::Specifier::Stop  : s = "stop"; break;
        case NetworkCommand::Specifier::Halt  : s = "halt"; break;
        case NetworkCommand::Specifier::Reboot: s = "reboot"; break;
        case NetworkCommand::Specifier::Reset : s = "reset"; break;
    };

    return s;
}

} // namespace cc::can_open
