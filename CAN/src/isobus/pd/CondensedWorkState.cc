/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/isobus/pd/CondensedWorkState>
#include <cc/Format>

namespace cc::isobus::pd {

String CondensedWorkState::toString() const
{
    Format f;
    f << "{\n";
    for (int i = 0; i < 16; ++i) {
        SwitchState state = sectionState(i);
        if (state != SwitchState::Undef) {
            String value;
            switch (state) {
                case SwitchState::On: value = "on"; break;
                case SwitchState::Off: value = "off"; break;
                case SwitchState::Error: value = "error"; break;
                default:;
            };
            f << "  \"section_" << sectionOffset_ + i << "\": \"" << value << "\",\n";
        }
    }
    f << "}";
    return f;
}

} // namespace cc::isobus::pd
