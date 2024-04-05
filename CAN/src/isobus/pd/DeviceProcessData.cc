/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/DeviceProcessData>
#include <cc/Format>

namespace cc::isobus::pd {

String str(DeviceProcessData::Properties properties)
{
    List<String> l;
    if (properties & DeviceProcessData::Properties::MemberOfDefaultSet) l << "\"member_of_default_set\"";
    if (properties & DeviceProcessData::Properties::Settable) l << "\"settable\"";
    if (properties & DeviceProcessData::Properties::ControlSource) l << "\"control_source\"";

    String s;
    if (l.count() > 0) s = "[ " + l.join(", ") + " ]";
    else s = "[]";
    return s;
}

String str(DeviceProcessData::TriggerMethods methods)
{
    List<String> l;
    if (methods & DeviceProcessData::TriggerMethods::TimeInterval) l << "\"time_interval\"";
    if (methods & DeviceProcessData::TriggerMethods::DistanceInterval) l << "\"distance_interval\"";
    if (methods & DeviceProcessData::TriggerMethods::ThresholdLimits) l << "\"threshold_limits\"";
    if (methods & DeviceProcessData::TriggerMethods::OnChange) l << "\"on_change\"";
    if (methods & DeviceProcessData::TriggerMethods::Total) l << "\"total\"";

    String s;
    if (l.count() > 0) s = "[ " + l.join(", ") + " ]";
    else s = "[]";
    return s;
}

String DeviceProcessData::toString() const
{
    return Format{} <<
        "{\n"
        "  \"type\": \"" << shortName() << "\",\n"
        "  \"object-id\": " << objectId() << ",\n"
        "  \"designator\": \"" << designator() << "\",\n"
        "  \"properties\": " << properties() << ",\n"
        "  \"trigger-methods\": " << triggerMethods() << ",\n"
        "  \"presentation-id\": " << presentationId() << "\n"
        "}";
}

} // namespace cc::isobus::pd
