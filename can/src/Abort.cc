/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/Abort>

namespace cc {
namespace can {

Ref<CanFrame> Abort::createFrame(int canId, uint8_t cs, Selector selector, Abort::Reason reason)
{
    auto frame = CanFrame::create();
    frame->setCanId(canId);
    frame->payloadAt(0) = cs << 5;
    frame->payloadAt(1) = selector->index();
    frame->payloadAt(2) = selector->index() >> 8;
    frame->payloadAt(3) = selector->subIndex();
    frame->setWord(4, 8, static_cast<uint32_t>(reason));
    return frame;
}

String str(Abort::Reason reason)
{
    String s;

    switch (reason) {
        case Abort::Reason::None                       : s = "Abort::Reason::None"; break;
        case Abort::Reason::ToggleBitInvalid           : s = "Abort::Reason::ToggleBitInvalid"; break;
        case Abort::Reason::ServiceDataTimeout         : s = "Abort::Reason::ServiceDataTimeout"; break;
        case Abort::Reason::SpecifierInvalid           : s = "Abort::Reason::SpecifierInvalid"; break;
        case Abort::Reason::BlockSizeInvalid           : s = "Abort::Reason::BlockSizeInvalid"; break;
        case Abort::Reason::SequenceNumberInvalid      : s = "Abort::Reason::SequenceNumberInvalid"; break;
        case Abort::Reason::CrcError                   : s = "Abort::Reason::CrcError"; break;
        case Abort::Reason::OutOfMemory                : s = "Abort::Reason::OutOfMemory"; break;
        case Abort::Reason::AccessDenied               : s = "Abort::Reason::AccessDenied"; break;
        case Abort::Reason::ReadAccessDenied           : s = "Abort::Reason::ReadAccessDenied"; break;
        case Abort::Reason::WriteAccessDenied          : s = "Abort::Reason::WriteAccessDenied"; break;
        case Abort::Reason::SelectorInvalid            : s = "Abort::Reason::SelectorInvalid"; break;
        case Abort::Reason::ProcessDataMappingFailed   : s = "Abort::Reason::ProcessDataMappingFailed"; break;
        case Abort::Reason::ProcessDataMappingOverflow : s = "Abort::Reason::ProcessDataMappingOverflow"; break;
        case Abort::Reason::CompatibilityError         : s = "Abort::Reason::CompatibilityError"; break;
        case Abort::Reason::InternalCompatibilityError : s = "Abort::Reason::InternalCompatibilityError"; break;
        case Abort::Reason::HardwareAccessFailed       : s = "Abort::Reason::HardwareAccessFailed"; break;
        case Abort::Reason::DataTypeLengthMismatch     : s = "Abort::Reason::DataTypeLengthMismatch"; break;
        case Abort::Reason::DataTypeLengthExceeded     : s = "Abort::Reason::DataTypeLengthExceeded"; break;
        case Abort::Reason::DataTypeLengthUndercut     : s = "Abort::Reason::DataTypeLengthUndercut"; break;
        case Abort::Reason::SubIndexInvalid            : s = "Abort::Reason::SubIndexInvalid"; break;
        case Abort::Reason::ValueOutOfBounds           : s = "Abort::Reason::ValueOutOfBounds"; break;
        case Abort::Reason::MaxValueExeeded            : s = "Abort::Reason::MaxValueExeeded"; break;
        case Abort::Reason::MinValueUndercut           : s = "Abort::Reason::MinValueUndercut"; break;
        case Abort::Reason::MinMaxBoundsError          : s = "Abort::Reason::MinMaxBoundsError"; break;
        case Abort::Reason::RessourceUnavailable       : s = "Abort::Reason::RessourceUnavailable"; break;
        case Abort::Reason::GeneralError               : s = "Abort::Reason::GeneralError"; break;
        case Abort::Reason::DataTransferError          : s = "Abort::Reason::DataTransferError"; break;
        case Abort::Reason::DataTransferLocalError     : s = "Abort::Reason::DataTransferLocalError"; break;
        case Abort::Reason::DataTransferDeviceError    : s = "Abort::Reason::DataTransferDeviceError"; break;
        case Abort::Reason::ObjectDictionaryUnavailable: s = "Abort::Reason::ObjectDictionaryUnavailable"; break;
        case Abort::Reason::DataUnavailable            : s = "Abort::Reason::DataUnavailable"; break;
    };

    return s;
}

}} // namespace cc::can
