/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/VendorInfo>
#include <cc/j1939/DictionaryManager>
#include <cc/j1939/FunctionId>

namespace cc::j1939 {

VendorInfo::VendorInfo(FunctionId functionId)
{
    DictionaryManager{}.vendorDictionary().lookup(functionId.vendor(), this);
}

} // namespace cc::j1939
