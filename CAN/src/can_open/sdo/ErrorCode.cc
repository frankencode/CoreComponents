/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/ErrorCode>

namespace cc::can_open::sdo {

String str(ErrorCode error)
{
    String s;

    switch (error) {
        case ErrorCode::None                       : s = "none"; break;
        case ErrorCode::ToggleBitInvalid           : s = "toggle bit invalid"; break;
        case ErrorCode::ServiceDataTimeout         : s = "service data timeout"; break;
        case ErrorCode::SpecifierInvalid           : s = "specifier invalid"; break;
        case ErrorCode::BlockSizeInvalid           : s = "block size invalid"; break;
        case ErrorCode::SequenceNumberInvalid      : s = "sequence number invalid"; break;
        case ErrorCode::CrcError                   : s = "CRC error"; break;
        case ErrorCode::OutOfMemory                : s = "out of memory"; break;
        case ErrorCode::AccessDenied               : s = "access denied"; break;
        case ErrorCode::ReadAccessDenied           : s = "read access denied"; break;
        case ErrorCode::WriteAccessDenied          : s = "write access denied"; break;
        case ErrorCode::SelectorInvalid            : s = "selector invalid"; break;
        case ErrorCode::ProcessDataMappingFailed   : s = "process data mapping failed"; break;
        case ErrorCode::ProcessDataMappingOverflow : s = "process data mapping overflow"; break;
        case ErrorCode::CompatibilityError         : s = "compatibility error"; break;
        case ErrorCode::InternalCompatibilityError : s = "internal compatibility error"; break;
        case ErrorCode::HardwareAccessFailed       : s = "hardware access failed"; break;
        case ErrorCode::DataTypeLengthMismatch     : s = "data type length mismatch"; break;
        case ErrorCode::DataTypeLengthExceeded     : s = "data type length exceeded"; break;
        case ErrorCode::DataTypeLengthUndercut     : s = "data type length undercut"; break;
        case ErrorCode::SubIndexInvalid            : s = "sub-index invalid"; break;
        case ErrorCode::ValueOutOfBounds           : s = "value out of bounds"; break;
        case ErrorCode::MaxValueExeeded            : s = "maximum value exeeded"; break;
        case ErrorCode::MinValueUndercut           : s = "minimum value undercut"; break;
        case ErrorCode::MinMaxBoundsError          : s = "out of bounds error"; break;
        case ErrorCode::RessourceUnavailable       : s = "ressource unavailable"; break;
        case ErrorCode::GeneralError               : s = "general error"; break;
        case ErrorCode::DataTransferError          : s = "data transfer error"; break;
        case ErrorCode::DataTransferLocalError     : s = "data transfer local error"; break;
        case ErrorCode::DataTransferDeviceError    : s = "data transfer device error"; break;
        case ErrorCode::ObjectDictionaryUnavailable: s = "object dictionary unavailable"; break;
        case ErrorCode::DataUnavailable            : s = "data unavailable"; break;
    };

    return s;
}

} // namespace cc::can_open::sdo
