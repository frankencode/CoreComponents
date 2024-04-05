/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/DesignatorChangeReply>
#include <cc/Format>

namespace cc::isobus::pd {

void DesignatorChangeReply::printValue(Format &f) const
{
    f <<
        "  \"object-id\": " << objectId() << ",\n"
        "  \"success\": " << success() << ",\n";
}

} // namespace cc::isobus::pd
