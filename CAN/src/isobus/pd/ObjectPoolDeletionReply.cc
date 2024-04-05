/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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
