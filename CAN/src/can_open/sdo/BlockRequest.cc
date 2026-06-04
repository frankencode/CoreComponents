/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/can_open/sdo/BlockRequest>
#include <cc/can_open/sdo/BlockWriteInitRequest>
#include <cc/can_open/sdo/BlockWriteEndRequest>
#include <cc/can_open/sdo/BlockReadInitRequest>
#include <cc/can_open/sdo/BlockReadEndRequest>
#include <cc/can_open/sdo/BlockReadStartRequest>
#include <cc/can_open/sdo/BlockReadAckRequest>

namespace cc::can_open::sdo {

String BlockRequest::toString() const
{
    if (!isValid()) return String{};

    String s;
    switch (specifier()) {
        case Specifier::BlockWriteStreamInit:
        case Specifier::BlockWriteInit: s = BlockWriteInitRequest{frame_}.toString(); break;
        case Specifier::BlockWriteEnd: s = BlockWriteEndRequest{frame_}.toString(); break;
        case Specifier::BlockReadInit: s = BlockReadInitRequest{frame_}.toString(); break;
        case Specifier::BlockReadEnd: s = BlockReadEndRequest{frame_}.toString(); break;
        case Specifier::BlockReadStart: s = BlockReadStartRequest{frame_}.toString(); break;
        case Specifier::BlockReadAck: s = BlockReadAckRequest{frame_}.toString(); break;
    };

    return s;
}

} // namespace cc::can_open::sdo
