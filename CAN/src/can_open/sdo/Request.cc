/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/Request>
#include <cc/can_open/sdo/ReadRequest>
#include <cc/can_open/sdo/WriteRequest>
#include <cc/can_open/sdo/ReadSegmentRequest>
#include <cc/can_open/sdo/WriteSegmentRequest>
#include <cc/can_open/sdo/AbortRequest>
#include <cc/can_open/sdo/BlockRequest>

namespace cc::can_open::sdo {

String Request::toString() const
{
    if (!isValid()) return String{};

    String s;

    switch (specifier()) {
        case Specifier::Read:         s = ReadRequest{frame_}.toString(); break;
        case Specifier::Write:        s = WriteRequest{frame_}.toString(); break;
        case Specifier::ReadSegment:  s = ReadSegmentRequest{frame_}.toString(); break;
        case Specifier::WriteSegment: s = WriteSegmentRequest{frame_}.toString(); break;
        case Specifier::Abort:        s = AbortRequest{frame_}.toString(); break;
        case Specifier::BlockRead:
        case Specifier::BlockWrite:   s = BlockRequest{frame_}.toString(); break;
    };

    return s;
}

String str(const Request &message)
{
    return message.toString();
}

} // namespace cc::can_open::sdo
