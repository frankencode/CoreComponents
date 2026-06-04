/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
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
