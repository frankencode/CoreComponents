/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanError>
#include <cc/Format>

namespace cc {
namespace can {

String str(CanError::Type flags)
{
    if (flags == CanError::Type::None) return "CanError::Type::None";

    auto parts = StringList::create();

    if (+(flags & CanError::Type::TransmissionTimeout)) parts << "CanError::Type::TransmissionTimeout";
    if (+(flags & CanError::Type::LostArbitration))     parts << "CanError::Type::LostArbitration";
    if (+(flags & CanError::Type::ControllerError))     parts << "CanError::Type::ControllerError";
    if (+(flags & CanError::Type::ProtocolViolation))   parts << "CanError::Type::ProtocolViolation";
    if (+(flags & CanError::Type::TranceiverStatus))    parts << "CanError::Type::TranceiverStatus";
    if (+(flags & CanError::Type::BussOff))             parts << "CanError::Type::BusOff";
    if (+(flags & CanError::Type::BusError))            parts << "CanError::Type::BusError";
    if (+(flags & CanError::Type::ControllerRestarted)) parts << "CanError::Type::ControllerRestarted";

    return parts->join("|");
}

String str(CanError::Status flags)
{
    if (flags == CanError::Status::None) return "CanError::Status::None";

    auto parts = StringList::create();

    if (+(flags & CanError::Status::RxOverflow))  parts << "CanError::Status::RxOverflow";
    if (+(flags & CanError::Status::TxOverflow))  parts << "CanError::Status::TxOverflow";
    if (+(flags & CanError::Status::RxWarning))   parts << "CanError::Status::RxWarning";
    if (+(flags & CanError::Status::TxWarning))   parts << "CanError::Status::TxWarning";
    if (+(flags & CanError::Status::RxPassive))   parts << "CanError::Status::RxPassive";
    if (+(flags & CanError::Status::TxPassive))   parts << "CanError::Status::TxPassive";
    if (+(flags & CanError::Status::ErrorActive)) parts << "CanError::Status::ErrorActive";

    return parts->join("|");
}

String CanError::Instance::toString() const
{
    Format f;
    f << "CanError {\n";
    f << "  type: " << type() << nl;
    if (+(type() & Type::LostArbitration)) f << "  bitPos: " << bitPos() << nl;
    if (+(type() & Type::ControllerError)) f << "  status: " << status() << nl;
    f << "}";
    return f;
}

}} // namespace cc::can
