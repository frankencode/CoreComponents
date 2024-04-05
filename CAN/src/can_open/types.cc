/*
 * Copyright (C) 2019-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/types>
#include <cc/Format>

namespace cc::can_open {

String str(NodeState state)
{
    String s;

    switch (state) {
        case NodeState::BootUp        : s = "NodeState::BootUp"; break;
        case NodeState::Stopped       : s = "NodeState::Stopped"; break;
        case NodeState::Operational   : s = "NodeState::Operational"; break;
        case NodeState::PreOperational: s = "NodeState::PreOperational"; break;
        case NodeState::Undefined     : s = "NodeState::Undefined"; break;
    };

    return s;
}

String str(TransferMode mode)
{
    String s;

    switch (mode) {
        case TransferMode::Segmented: s = "TransferMode::Segmented"; break;
        case TransferMode::Expedited: s = "TransferMode::Expedited"; break;
        case TransferMode::Block    : s = "TransferMode::Block"; break;
    };

    return s;
}

String str(ErrorRegister flags)
{
    if (flags == ErrorRegister::None) return "[]";

    List<String> parts;

    parts << "[ ";

    if (+(flags & ErrorRegister::Generic      )) parts << "\"generic\"";
    if (+(flags & ErrorRegister::Current      )) parts << "\"current\"";
    if (+(flags & ErrorRegister::Voltage      )) parts << "\"voltage\"";
    if (+(flags & ErrorRegister::Temperature  )) parts << "\"temperature\"";
    if (+(flags & ErrorRegister::Communication)) parts << "\"communication\"";
    if (+(flags & ErrorRegister::DeviceProfile)) parts << "\"device profile\"";
    if (+(flags & ErrorRegister::Reserved     )) parts << "\"reserved\"";
    if (+(flags & ErrorRegister::Custom       )) parts << "\"custom\"";

    parts << " ]";

    return parts.join(", ");
}

String str(SafetyError error)
{
    String s;

    switch (error) {
        case SafetyError::DataValidation   : s = "SafetyError::DataValidation"; break;
        case SafetyError::RefreshTimeout   : s = "SafetyError::RefreshTimeout"; break;
        case SafetyError::ValidationTimeout: s = "SafetyError::ValidationTimeout"; break;
    };

    return s;
}

String str(DataType type)
{
    String s;

    switch (type) {
        case DataType::Boolean             : s = "DataType::Boolean"; break;
        case DataType::Integer8            : s = "DataType::Integer8"; break;
        case DataType::Integer16           : s = "DataType::Integer16"; break;
        case DataType::Integer32           : s = "DataType::Integer32"; break;
        case DataType::Unsigned8           : s = "DataType::Unsigned8"; break;
        case DataType::Unsigned16          : s = "DataType::Unsigned16"; break;
        case DataType::Unsigned32          : s = "DataType::Unsigned32"; break;
        case DataType::Real32              : s = "DataType::Real32"; break;
        case DataType::VisibleString       : s = "DataType::VisibleString"; break;
        case DataType::OctetString         : s = "DataType::OctetString"; break;
        case DataType::UnicodeString       : s = "DataType::UnicodeString"; break;
        case DataType::TimeOfDay           : s = "DataType::TimeOfDay"; break;
        case DataType::TimeDifference      : s = "DataType::TimeDifference"; break;
        case DataType::Domain              : s = "DataType::Domain"; break;
        case DataType::Integer24           : s = "DataType::Integer24"; break;
        case DataType::Real64              : s = "DataType::Real64"; break;
        case DataType::Unsigned40          : s = "DataType::Unsigned40"; break;
        case DataType::Unsigned48          : s = "DataType::Unsigned48"; break;
        case DataType::Unsigned56          : s = "DataType::Unsigned56"; break;
        case DataType::Unsigned64          : s = "DataType::Unsigned64"; break;
        case DataType::ProcessDataParameter: s = "DataType::ProcessDataParameter"; break;
        case DataType::ProcessDataMapping  : s = "DataType::ProcessDataMapping"; break;
        case DataType::DictionaryParameter : s = "DataType::DictionaryParameter"; break;
        case DataType::Identity            : s = "DataType::Identity"; break;
        default: s = Format{"DataType(%%)"} << static_cast<uint16_t>(type); break;
    };

    return s;
}

String str(ObjectCode code)
{
    String s;

    switch (code) {
        case ObjectCode::Null     : s = "ObjectCode::Null"; break;
        case ObjectCode::Domain   : s = "ObjectCode::Domain"; break;
        case ObjectCode::DefType  : s = "ObjectCode::DefType"; break;
        case ObjectCode::DefStruct: s = "ObjectCode::DefStruct"; break;
        case ObjectCode::Var      : s = "ObjectCode::Var"; break;
        case ObjectCode::Array    : s = "ObjectCode::Array"; break;
        case ObjectCode::Record   : s = "ObjectCode::Record"; break;
    };

    return s;
}

String str(AccessType access)
{
    String s;

    switch (access) {
        case AccessType::Undefined    : s = "AccessType::Undefined"; break;
        case AccessType::ReadOnly     : s = "AccessType::ReadOnly"; break;
        case AccessType::WriteOnly    : s = "AccessType::WriteOnly"; break;
        case AccessType::ReadWrite    : s = "AccessType::ReadWrite"; break;
        case AccessType::ProcessInput : s = "AccessType::ProcessInput"; break;
        case AccessType::ProcessOutput: s = "AccessType::ProcessOutput"; break;
    };

    return s;
}

String str(ObjectFlags flags)
{
    if (flags == ObjectFlags::None) return "ObjectFlags::None";

    List<String> parts;
    if (+(flags & ObjectFlags::IgnoreOnNodeInit)) parts << "ObjectFlags::IgnoreOnNodeInit";
    if (+(flags & ObjectFlags::IgnoreOnNodeScan)) parts << "ObjectFlags::IgnoreOnNodeScan";

    return parts.join("|");
}

String hexLine(const Bytes &data)
{
    String visual = String::allocate(data.count());
    for (int i = 0; i < visual.count(); ++i) {
        if (data.at(i) < 0x20 || 0x7E < data.at(i)) {
            visual.at(i) = '.';
        }
        else {
            visual.at(i) = data.at(i);
        }
    }

    return Format{}
        << hex(data).breakUp(2).join(".")
        << "    "
        << "|" << visual << "|";
}

} // namespace cc::can_open
