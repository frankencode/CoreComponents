/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/types>
#include <cc/Format>

namespace cc {
namespace can {

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
    if (flags == ErrorRegister::None) return "ErrorRegister::None";

    auto parts = StringList::create();

    if (+(flags & ErrorRegister::Generic      )) parts << "ErrorRegister::Generic";
    if (+(flags & ErrorRegister::Current      )) parts << "ErrorRegister::Current";
    if (+(flags & ErrorRegister::Voltage      )) parts << "ErrorRegister::Voltage";
    if (+(flags & ErrorRegister::Temperature  )) parts << "ErrorRegister::Temperature";
    if (+(flags & ErrorRegister::Communication)) parts << "ErrorRegister::Communication";
    if (+(flags & ErrorRegister::DeviceProfile)) parts << "ErrorRegister::DeviceProfile";
    if (+(flags & ErrorRegister::Reserved     )) parts << "ErrorRegister::Reserved";
    if (+(flags & ErrorRegister::Custom       )) parts << "ErrorRegister::Custom";

    return parts->join("|");
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
        default: s = "DataType(" + str(static_cast<uint16_t>(type)) + ")"; break;
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

    auto parts = StringList::create();
    if (+(flags & ObjectFlags::IgnoreOnNodeInit)) parts << "ObjectFlags::IgnoreOnNodeInit";
    if (+(flags & ObjectFlags::IgnoreOnNodeScan)) parts << "ObjectFlags::IgnoreOnNodeScan";

    return parts->join("|");

}

String hexLine(const String &data)
{
    String visual = data->copy();
    for (int i = 0; i < visual->count(); ++i) {
        if (visual->byteAt(i) < 0x20 || 0x7E < visual->byteAt(i))
            mutate(visual)->at(i) = '.';
    }

    return Format{}
        << data->toHex()->breakUp(2)->join(".")
        << "    "
        << "|" << visual << "|";
}

}} // namespace cc::can
