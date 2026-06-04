/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/isobus/pd/ObjectPoolActivationRequest>
#include <cc/Format>

namespace cc::isobus::pd {

void ObjectPoolActivationRequest::printValue(Format &f) const
{
    f << "  \"command\": \"" << (command() == Command::Activate ? "Activate" : "Deactivate") << "\",\n";
}

} // namespace cc::isobus::pd
