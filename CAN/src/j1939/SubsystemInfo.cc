/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/SubsystemInfo>
#include <cc/j1939/DictionaryManager>
#include <cc/j1939/FunctionId>

namespace cc::j1939 {

SubsystemInfo::SubsystemInfo(FunctionId functionId)
{
    DictionaryManager{}.subsystemDictionary().lookup(functionId.industry(), functionId.subsystem(), this);
}

} // namespace cc::j1939
