/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/CanId>
#include <linux/can.h>

namespace cc::j1939 {

CanId::operator uint32_t() const { return canId_ | CAN_EFF_FLAG; }

} // namespace cc::j1939
