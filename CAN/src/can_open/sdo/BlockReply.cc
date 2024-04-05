/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/BlockReply>
#include <cc/can_open/sdo/BlockWriteInitReply>
#include <cc/can_open/sdo/BlockWriteEndReply>
#include <cc/can_open/sdo/BlockWriteAckReply>
#include <cc/can_open/sdo/BlockReadInitReply>
#include <cc/can_open/sdo/BlockReadEndReply>

namespace cc::can_open::sdo {

String BlockReply::toString() const
{
    if (!isValid()) return String{};

    String s;
    switch (specifier()) {
        case Specifier::BlockWriteInit     : s = BlockWriteInitReply{frame_}.toString(); break;
        case Specifier::BlockWriteEnd      : s = BlockWriteEndReply{frame_}.toString(); break;
        case Specifier::BlockWriteAck      : s = BlockWriteAckReply{frame_}.toString(); break;
        case Specifier::BlockReadStreamInit:
        case Specifier::BlockReadInit      : s = BlockReadInitReply{frame_}.toString(); break;
        case Specifier::BlockReadEnd       : s = BlockReadEndReply{frame_}.toString(); break;
        default: break;
    };

    return s;
}

} // namespace cc::can_open::sdo
