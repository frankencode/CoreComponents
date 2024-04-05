/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/FunctionId>
#include <cc/j1939/SubsystemInfo>
#include <cc/j1939/FunctionInfo>
#include <cc/j1939/VendorInfo>
#include <cc/j1939/IndustryInfo>
#include <cc/Format>

namespace cc::j1939 {

String FunctionId::toString() const
{
    FunctionId self = *this;

    SubsystemInfo subsystemInfo{self};

    FunctionInfo functionInfo;
    if (subsystemInfo) subsystemInfo.functionInfos().lookup(static_cast<int>(self.function()), &functionInfo);

    VendorInfo vendorInfo{self};
    IndustryInfo industryInfo{self};

    Format f;
    f <<
        "{\n"
        "  \"address-claiming\": " << static_cast<bool>(self.addressClaiming()) << ",\n";

    if (vendorInfo) {
        f << "  \"vendor\": \"" << vendorInfo.name() << "\",\n";
    }
    else {
        f << "  \"vendor\": " << self.vendor() << ",\n";
    }

    if (industryInfo) {
        f << "  \"industry\": \"" << industryInfo.name() << "\",\n";
    }
    else {
        f << "  \"industry\": " << self.industry() << ",\n";
    }

    if (subsystemInfo) {
        f << "  \"subsystem\": \"" << subsystemInfo.name() << "\",\n";
    }
    else {
        f << "  \"subsystem\": " << self.subsystem() << ",\n";
    }

    if (functionInfo) {
        f << "  \"function\": \"" << functionInfo.name() << "\",\n";
    }
    else {
        f << "  \"function\": " << self.function() << ",\n";
    }

    f <<
        "  \"subsystem-instance\": " << self.subsystemInstance() << ",\n"
        "  \"ecu-instance\": " << self.ecuInstance() << ",\n"
        "  \"serial-number\": " << self.serial() << "\n"
        "}";

    return f;
}

String FunctionId::functionName() const
{
    FunctionId self = *this;
    SubsystemInfo subsystemInfo{self};

    FunctionInfo functionInfo;
    if (subsystemInfo) subsystemInfo.functionInfos().lookup(static_cast<int>(self.function()), &functionInfo);

    if (!functionInfo) {
        VendorInfo vendorInfo{self};
        if (vendorInfo) {
            return vendorInfo.name().split(' ')(0);
        }
    }

    return functionInfo ? functionInfo.name() : String{"Unknown"};
}

} // namespace cc::j1939
