/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/j1939/CanId>
#include <linux/can.h>

namespace cc::j1939 {

CanId::operator uint32_t() const { return canId_ | CAN_EFF_FLAG; }

} // namespace cc::j1939
