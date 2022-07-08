/*
 * Copyright (C) 2019-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/CanError>
#include <linux/can/error.h>

namespace cc {

static_assert(+CanError::Type::TxTimeout         == CAN_ERR_TX_TIMEOUT);
static_assert(+CanError::Type::LostArbitration   == CAN_ERR_LOSTARB);
static_assert(+CanError::Type::ControllerError   == CAN_ERR_CRTL);
static_assert(+CanError::Type::ProtocolViolation == CAN_ERR_PROT);
static_assert(+CanError::Type::TxStatus          == CAN_ERR_TRX);
static_assert(+CanError::Type::AckMissing        == CAN_ERR_ACK);
static_assert(+CanError::Type::BusOff            == CAN_ERR_BUSOFF);
static_assert(+CanError::Type::BusError          == CAN_ERR_BUSERROR);
static_assert(+CanError::Type::Restarted         == CAN_ERR_RESTARTED);

static_assert(+CanError::Controller::Unspec      == CAN_ERR_CRTL_UNSPEC);
static_assert(+CanError::Controller::RxOverflow  == CAN_ERR_CRTL_RX_OVERFLOW);
static_assert(+CanError::Controller::TxOverflow  == CAN_ERR_CRTL_TX_OVERFLOW);
static_assert(+CanError::Controller::RxWarning   == CAN_ERR_CRTL_RX_WARNING);
static_assert(+CanError::Controller::TxWarning   == CAN_ERR_CRTL_TX_WARNING);
static_assert(+CanError::Controller::RxPassive   == CAN_ERR_CRTL_RX_PASSIVE);
static_assert(+CanError::Controller::TxPassive   == CAN_ERR_CRTL_TX_PASSIVE);
static_assert(+CanError::Controller::ErrorActive == CAN_ERR_CRTL_ACTIVE);

static_assert(+CanError::Protocol::Unspec      == CAN_ERR_PROT_UNSPEC);
static_assert(+CanError::Protocol::SingleBit   == CAN_ERR_PROT_BIT);
static_assert(+CanError::Protocol::FrameFormat == CAN_ERR_PROT_FORM);
static_assert(+CanError::Protocol::BitStuffing == CAN_ERR_PROT_STUFF);
static_assert(+CanError::Protocol::Bit0        == CAN_ERR_PROT_BIT0);
static_assert(+CanError::Protocol::Bit1        == CAN_ERR_PROT_BIT1);
static_assert(+CanError::Protocol::Overload    == CAN_ERR_PROT_OVERLOAD);
static_assert(+CanError::Protocol::Active      == CAN_ERR_PROT_ACTIVE);
static_assert(+CanError::Protocol::Tx          == CAN_ERR_PROT_TX);

static_assert(+CanError::Wire::Unspec         == CAN_ERR_TRX_UNSPEC);
static_assert(+CanError::Wire::DisconnectHigh == CAN_ERR_TRX_CANH_NO_WIRE);
static_assert(+CanError::Wire::DisconnectLow  == CAN_ERR_TRX_CANL_NO_WIRE);
static_assert(+CanError::Wire::ShortHighToBat == CAN_ERR_TRX_CANH_SHORT_TO_BAT);
static_assert(+CanError::Wire::ShortHighToVcc == CAN_ERR_TRX_CANH_SHORT_TO_VCC);
static_assert(+CanError::Wire::ShortHighToGnd == CAN_ERR_TRX_CANH_SHORT_TO_GND);
static_assert(+CanError::Wire::ShortLowToBat  == CAN_ERR_TRX_CANL_SHORT_TO_BAT);
static_assert(+CanError::Wire::ShortLowToVcc  == CAN_ERR_TRX_CANL_SHORT_TO_VCC);
static_assert(+CanError::Wire::ShortLowToGnd  == CAN_ERR_TRX_CANL_SHORT_TO_GND);
static_assert(+CanError::Wire::ShortLowToHigh == CAN_ERR_TRX_CANL_SHORT_TO_CANH);

String str(CanError::Type type)
{
    List<String> parts;
    if (type & CanError::Type::TxTimeout        ) parts << "CanError::Type::TxTimeout";
    if (type & CanError::Type::LostArbitration  ) parts << "CanError::Type::LostArbitration";
    if (type & CanError::Type::ControllerError  ) parts << "CanError::Type::ControllerError";
    if (type & CanError::Type::ProtocolViolation) parts << "CanError::Type::ProtocolViolation";
    if (type & CanError::Type::TxStatus         ) parts << "CanError::Type::TxStatus";
    if (type & CanError::Type::BusOff           ) parts << "CanError::Type::BusOff";
    if (type & CanError::Type::BusError         ) parts << "CanError::Type::BusError";
    if (type & CanError::Type::Restarted        ) parts << "CanError::Type::Restarted";
    if (parts.count() == 0) return "CanError::Type::None";
    return parts.join('|');
}

String str(CanError::Controller status)
{
    List<String> parts;
    if (status & CanError::Controller::RxOverflow ) parts << "CanError::Controller::RxOverflow";
    if (status & CanError::Controller::TxOverflow ) parts << "CanError::Controller::TxOverflow";
    if (status & CanError::Controller::RxWarning  ) parts << "CanError::Controller::RxWarning";
    if (status & CanError::Controller::TxOverflow ) parts << "CanError::Controller::TxOverflow";
    if (status & CanError::Controller::RxWarning  ) parts << "CanError::Controller::RxWarning";
    if (status & CanError::Controller::TxWarning  ) parts << "CanError::Controller::TxWarning";
    if (status & CanError::Controller::RxPassive  ) parts << "CanError::Controller::RxPassive";
    if (status & CanError::Controller::TxPassive  ) parts << "CanError::Controller::TxPassive";
    if (status & CanError::Controller::ErrorActive) parts << "CanError::Controller::ErrorActive";
    if (parts.count() == 0) return "CanError::Controller::Unspec";
    return parts.join('|');
}

String str(CanError::Protocol status)
{
    List<String> parts;
    if (status & CanError::Protocol::SingleBit  ) parts << "CanError::Protocol::SingleBit";
    if (status & CanError::Protocol::FrameFormat) parts << "CanError::Protocol::FrameFormat";
    if (status & CanError::Protocol::BitStuffing) parts << "CanError::Protocol::BitStuffing";
    if (status & CanError::Protocol::Bit0       ) parts << "CanError::Protocol::Bit0";
    if (status & CanError::Protocol::Bit1       ) parts << "CanError::Protocol::Bit1";
    if (status & CanError::Protocol::Overload   ) parts << "CanError::Protocol::Overload";
    if (status & CanError::Protocol::Active     ) parts << "CanError::Protocol::Active";
    if (status & CanError::Protocol::Tx         ) parts << "CanError::Protocol::Tx";
    if (parts.count() == 0) return "CanError::Protocol::Unspec";
    return parts.join('|');
}

String str(CanError::Wire status)
{
    List<String> parts;
    if (status & CanError::Wire::DisconnectHigh) parts << "CanError::Wire::DisconnectHigh";
    if (status & CanError::Wire::DisconnectLow ) parts << "CanError::Wire::DisconnectLow";
    if (status & CanError::Wire::ShortHighToBat) parts << "CanError::Wire::ShortHighToBat";
    if (status & CanError::Wire::ShortLowToBat ) parts << "CanError::Wire::ShortLowToBat";
    if (status & CanError::Wire::ShortHighToVcc) parts << "CanError::Wire::ShortHighToVcc";
    if (status & CanError::Wire::ShortLowToVcc ) parts << "CanError::Wire::ShortLowToVcc";
    if (status & CanError::Wire::ShortHighToGnd) parts << "CanError::Wire::ShortHighToGnd";
    if (status & CanError::Wire::ShortLowToGnd ) parts << "CanError::Wire::ShortLowToGnd";
    if (status & CanError::Wire::ShortLowToHigh) parts << "CanError::Wire::ShortLowToHigh";
    if (parts.count() == 0) return "CanError::Controller::Unspec";
    return parts.join('|');
}

} // namespace cc
