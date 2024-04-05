/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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
