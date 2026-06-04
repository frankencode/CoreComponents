/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/isobus/pd/ObjectPoolTransferReply>
#include <cc/Format>

namespace cc::isobus::pd {

void ObjectPoolTransferReply::printValue(Format &f) const
{
    f << "  \"clear-to-send\": " << clearToSend() << ",\n";
}

} // namespace cc::isobus::pd
