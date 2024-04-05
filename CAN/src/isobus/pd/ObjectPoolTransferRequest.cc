/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/ObjectPoolTransferRequest>
#include <cc/Format>

namespace cc::isobus::pd {

void ObjectPoolTransferRequest::printValue(Format &f) const
{
    f << "  \"requested-transfer-size\": " << requestedTransferSize() << ",\n";
}

} // namespace cc::isobus::pd
