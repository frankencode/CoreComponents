/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/Reply>
#include <cc/can_open/sdo/ReadReply>
#include <cc/can_open/sdo/WriteReply>
#include <cc/can_open/sdo/ReadSegmentReply>
#include <cc/can_open/sdo/WriteSegmentReply>
#include <cc/can_open/sdo/AbortReply>
#include <cc/can_open/sdo/BlockReply>

namespace cc::can_open::sdo {

String Reply::toString() const
{
    if (!isValid()) return String{};

    String s;
    switch (specifier()) {
        case Specifier::Read: s = ReadReply{frame_}.toString(); break;
        case Specifier::Write: s = WriteReply{frame_}.toString(); break;
        case Specifier::ReadSegment: s = ReadSegmentReply{frame_}.toString(); break;
        case Specifier::WriteSegment: s = WriteSegmentReply{frame_}.toString(); break;
        case Specifier::Abort: s = AbortReply{frame_}.toString(); break;
        case Specifier::BlockRead:
        case Specifier::BlockWrite: s = BlockReply{frame_}.toString(); break;
        default: break;
    };

    return s;
}

String str(const Reply &message)
{
    return message.toString();
}

} // namespace cc::can_open::sdo
