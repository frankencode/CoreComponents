/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/CondensedWorkStateMessage>
#include <cc/Format>

namespace cc::isobus::pd {

void CondensedWorkStateMessage::printValue(Format &f) const
{
    uint8_t j = sectionOffset();
    for (int i = 0; i < 16; ++i) {
        SwitchState state = sectionState(i);
        if (state != SwitchState::Undef) {
            String value;
            if (367 <= valueType() && valueType() <= 382) {
                switch (state) {
                    case SwitchState::On: value = "overriden"; break;
                    case SwitchState::Off: value = "sc"; break;
                    case SwitchState::Error: value = "error"; break;
                    default:;
                };
            }
            else {
                switch (state) {
                    case SwitchState::On: value = "on"; break;
                    case SwitchState::Off: value = "off"; break;
                    case SwitchState::Error: value = "error"; break;
                    default:;
                };
            }
            f << "  \"section_" << j + i << "\": \"" << value << "\",\n";
        }
    }
}

} // namespace cc::isobus::pd
