/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/isobus/pd/ObjectPoolDeletionReply>
#include <cc/Format>

namespace cc::isobus::pd {

void ObjectPoolDeletionReply::printValue(Format &f) const
{
    f << "  \"success\": " << success() << nl;
}

} // namespace cc::isobus::pd
