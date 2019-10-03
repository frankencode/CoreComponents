/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/Emergency>
#include <cc/Format>

namespace cc {
namespace can {

Ref<CanFrame> Emergency::createFrame(int nodeId, Emergency::Type type, ErrorRegister errorRegister, uint64_t deviceError, int monitoredNodeId)
{
    auto frame = CanFrame::create();
    frame->setCanId(0x80 + nodeId);
    uint16_t errorCode = static_cast<uint16_t>(type) + ((type == Emergency::Type::CanKeepAliveForNode) ? monitoredNodeId : 0);
    frame->payloadAt(0) = errorCode & 0xFF;
    frame->payloadAt(1) = (errorCode >> 8) & 0xFF;
    frame->payloadAt(2) = static_cast<uint8_t>(errorRegister);
    for (int i = 3; i <= 7; ++i) {
        frame->payloadAt(i) = uint8_t(deviceError & 0xFF);
        deviceError >>= 8;
    }
    return frame;
}

String Emergency::Instance::toString() const
{
    if (!isValid()) return String{};

    return Format{}
        << "Emergency {" << nl
        << "  nodeId       : " << nodeId() << nl
        << "  type         : " << type() << nl
        << "  errorRegister: " << errorRegister() << "(" << static_cast<uint8_t>(errorRegister()) << ")" << nl
        << "  deviceError  : 0x" << hex(deviceError()) << nl
        << "}";
}

String str(Emergency::Type emergencyType)
{
    String s;

    switch (emergencyType) {
        case Emergency::Type::None               : s = "Emergency::Type::None"; break;
        case Emergency::Type::Generic            : s = "Emergency::Type::Generic"; break;
        case Emergency::Type::Current            : s = "Emergency::Type::Current"; break;
        case Emergency::Type::InputCurrent       : s = "Emergency::Type::InputCurrent"; break;
        case Emergency::Type::InsideCurrent      : s = "Emergency::Type::InsideCurrent"; break;
        case Emergency::Type::OutputCurrent      : s = "Emergency::Type::OutputCurrent"; break;
        case Emergency::Type::Voltage            : s = "Emergency::Type::Voltage"; break;
        case Emergency::Type::InputVoltage       : s = "Emergency::Type::InputVoltage"; break;
        case Emergency::Type::InsideVoltage      : s = "Emergency::Type::InsideVoltage"; break;
        case Emergency::Type::OutputVoltage      : s = "Emergency::Type::OutputVoltage"; break;
        case Emergency::Type::Temperature        : s = "Emergency::Type::Temperature"; break;
        case Emergency::Type::AmbientTemperature : s = "Emergency::Type::AmbientTemperature"; break;
        case Emergency::Type::DeviceTemperature  : s = "Emergency::Type::DeviceTemperature"; break;
        case Emergency::Type::Hardware           : s = "Emergency::Type::Hardware"; break;
        case Emergency::Type::Software           : s = "Emergency::Type::Software"; break;
        case Emergency::Type::InternalSoftware   : s = "Emergency::Type::InternalSoftware"; break;
        case Emergency::Type::UserSoftware       : s = "Emergency::Type::UserSoftware"; break;
        case Emergency::Type::DataSet            : s = "Emergency::Type::DataSet"; break;
        case Emergency::Type::AdditionalModules  : s = "Emergency::Type::AdditionalModules"; break;
        case Emergency::Type::Monitoring         : s = "Emergency::Type::Monitoring"; break;
        case Emergency::Type::Communication      : s = "Emergency::Type::Communication"; break;
        case Emergency::Type::CanOverrun         : s = "Emergency::Type::CanOverrun"; break;
        case Emergency::Type::CanPassiveMode     : s = "Emergency::Type::CanPassiveMode"; break;
        case Emergency::Type::CanKeepAlive       : s = "Emergency::Type::CanKeepAlive"; break;
        case Emergency::Type::CanBusOff          : s = "Emergency::Type::CanBusOff"; break;
        case Emergency::Type::CanIdCollision     : s = "Emergency::Type::CanIdCollision"; break;
        case Emergency::Type::ProtocolError      : s = "Emergency::Type::ProtocolError"; break;
        case Emergency::Type::ProcessDataLength  : s = "Emergency::Type::ProcessDataLength"; break;
        case Emergency::Type::ProcessDataOverhang: s = "Emergency::Type::ProcessDataOverhang"; break;
        case Emergency::Type::NoSuchObject       : s = "Emergency::Type::NoSuchObject"; break;
        case Emergency::Type::SyncOverhang       : s = "Emergency::Type::SyncOverhang"; break;
        case Emergency::Type::ProcessDataTimeout : s = "Emergency::Type::ProcessDataTimeout"; break;
        case Emergency::Type::External           : s = "Emergency::Type::External"; break;
        case Emergency::Type::AdditionalFunctions: s = "Emergency::Type::AdditionalFunctions"; break;
        case Emergency::Type::DeviceSpecific     : s = "Emergency::Type::DeviceSpecific"; break;
        case Emergency::Type::CanKeepAliveForNode: s = "Emergency::Type::CanKeepAliveForNode"; break;
        default: {
            s = Format("Emergency::Type::Custom(%%)") << static_cast<uint16_t>(emergencyType);
            break;
        }
    };

    return s;
}

}} // namespace cc::can
