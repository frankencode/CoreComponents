/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/ServiceReply>
#include <cc/can/ReadReply>
#include <cc/can/ReadSegmentReply>
#include <cc/can/WriteReply>
#include <cc/can/WriteSegmentReply>
#include <cc/can/AbortReply>
#include <cc/can/BlockReply>

namespace cc {
namespace can {

String ServiceReply::Instance::toString() const
{
    if (!isValid()) return String{};

    String s;
    switch (specifier()) {
        case Specifier::Read        : s = ReadReply{frame_}; break;
        case Specifier::Write       : s = WriteReply{frame_}; break;
        case Specifier::ReadSegment : s = ReadSegmentReply{frame_}; break;
        case Specifier::WriteSegment: s = WriteSegmentReply{frame_}; break;
        case Specifier::Abort       : s = AbortReply{frame_}; break;
        case Specifier::BlockRead   :
        case Specifier::BlockWrite  : s = BlockReply{frame_}; break;
        default: break;
    };

    return s;
}

}} // namespace cc::can
