/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/SaeMessage>
#include <cc/format>

namespace cc {
namespace can {

string SaeMessage::Instance::toString() const
{
    if (!isValid()) return string{};

    return format{}
        << "SaeMessage {" << nl
        << "  parameterGroupNumber: " << parameterGroupNumber() << nl
        << "  sourceAddress       : " << sourceAddress() << nl
        << "}";
}

}} // namespace cc::can
