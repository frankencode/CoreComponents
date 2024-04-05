/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/Emergency>
#include <cc/Format>
#include <cassert>

namespace cc::can_open {

CanFrame Emergency::createFrame(int nodeId, Emergency::Type type, ErrorRegister errorRegister, uint64_t deviceError, int monitoredNodeId)
{
    assert(1 <= nodeId && nodeId <= 0x7F);
    assert(static_cast<int>(type == Emergency::Type::CanKeepAliveForNode) <= monitoredNodeId && monitoredNodeId <= 0x7F);

    uint16_t errorCode = static_cast<uint16_t>(type) + ((type == Emergency::Type::CanKeepAliveForNode) ? monitoredNodeId : 0);

    return
        CanFrame { 0x80 + static_cast<uint32_t>(nodeId) }
        (0, 1, errorCode)
        (2, +errorRegister)
        (3, 7, deviceError);
}

String Emergency::toString() const
{
    if (!isValid()) return String{};

    return Format{} <<
        "{\n"
        "  \"type\": \"" << typeName() << ",\n"
        "  \"source\": \"" << nodeId() << ",\n"
        "  \"emergency-type\": \"" << +type() << ",\n"
        "  \"errors\": " << errorRegister() << ",\n"
        "  \"error-register\": " << +errorRegister() << ",\n"
        "  \"vendor-error\": 0x" << hex(vendorError()) << "\n"
        "}";
}

String str(Emergency::Type type)
{
    String s;

    switch (type) {
        case Emergency::Type::None               : s = "none"; break;
        case Emergency::Type::Generic            : s = "generic"; break;
        case Emergency::Type::Current            : s = "current"; break;
        case Emergency::Type::InputCurrent       : s = "input current"; break;
        case Emergency::Type::InsideCurrent      : s = "inside current"; break;
        case Emergency::Type::OutputCurrent      : s = "output current"; break;
        case Emergency::Type::Voltage            : s = "voltage"; break;
        case Emergency::Type::InputVoltage       : s = "input voltage"; break;
        case Emergency::Type::InsideVoltage      : s = "inside voltage"; break;
        case Emergency::Type::OutputVoltage      : s = "output voltage"; break;
        case Emergency::Type::Temperature        : s = "temperature"; break;
        case Emergency::Type::AmbientTemperature : s = "ambient temperature"; break;
        case Emergency::Type::DeviceTemperature  : s = "device temperature"; break;
        case Emergency::Type::Hardware           : s = "hardware"; break;
        case Emergency::Type::Software           : s = "software"; break;
        case Emergency::Type::InternalSoftware   : s = "internal software"; break;
        case Emergency::Type::UserSoftware       : s = "user software"; break;
        case Emergency::Type::DataSet            : s = "data set"; break;
        case Emergency::Type::AdditionalModules  : s = "additional modules"; break;
        case Emergency::Type::Monitoring         : s = "monitoring"; break;
        case Emergency::Type::Communication      : s = "communication"; break;
        case Emergency::Type::CanOverrun         : s = "CAN overrun"; break;
        case Emergency::Type::CanPassiveMode     : s = "CAN passive mode"; break;
        case Emergency::Type::CanKeepAlive       : s = "CAN keep-alive"; break;
        case Emergency::Type::CanBusOff          : s = "CAN bus off"; break;
        case Emergency::Type::CanIdCollision     : s = "CAN ID collision"; break;
        case Emergency::Type::ProtocolError      : s = "protocol error"; break;
        case Emergency::Type::ProcessDataLength  : s = "process data length"; break;
        case Emergency::Type::ProcessDataOverhang: s = "process data overhang"; break;
        case Emergency::Type::NoSuchObject       : s = "no such object"; break;
        case Emergency::Type::SyncOverhang       : s = "sync overhang"; break;
        case Emergency::Type::ProcessDataTimeout : s = "process data timeout"; break;
        case Emergency::Type::External           : s = "external"; break;
        case Emergency::Type::AdditionalFunctions: s = "additional functions"; break;
        case Emergency::Type::DeviceSpecific     : s = "device specific"; break;
        case Emergency::Type::CanKeepAliveForNode: s = "CAN keep-alive for node"; break;
        default: {
            s = Format{"unknown (%%)"} << static_cast<uint16_t>(type);
            break;
        }
    };

    return s;
}

} // namespace cc::can_open
