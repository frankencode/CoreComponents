/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/ServiceRequest>
#include <cc/can/ReadRequest>
#include <cc/can/WriteRequest>
#include <cc/can/ReadSegmentRequest>
#include <cc/can/WriteSegmentRequest>
#include <cc/can/AbortRequest>
#include <cc/can/BlockRequest>

namespace cc {
namespace can {

String ServiceRequest::Instance::toString() const
{
    if (!isValid()) return String{};

    String s;
    switch (specifier()) {
        case Specifier::Read:         s = ReadRequest{frame_}; break;
        case Specifier::Write:        s = WriteRequest{frame_}; break;
        case Specifier::ReadSegment:  s = ReadSegmentRequest{frame_}; break;
        case Specifier::WriteSegment: s = WriteSegmentRequest{frame_}; break;
        case Specifier::Abort:        s = AbortRequest{frame_}; break;
        case Specifier::BlockRead:
        case Specifier::BlockWrite:   s = BlockRequest{frame_}; break;
    };

    return s;
}

}} // namespace cc::can
