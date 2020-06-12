/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/BlockRequest>
#include <cc/can/BlockWriteInitRequest>
#include <cc/can/BlockWriteSegmentRequest>
#include <cc/can/BlockWriteEndRequest>
#include <cc/can/BlockReadInitRequest>
#include <cc/can/BlockReadStartRequest>
#include <cc/can/BlockReadAckRequest>
#include <cc/can/BlockReadEndRequest>

namespace cc {
namespace can {

String BlockRequest::Instance::toString() const
{
    if (!isValid()) return String{};

    String s;
    switch (specifier()) {
        case Specifier::BlockWriteStreamInit:
        case Specifier::BlockWriteInit      : s = BlockWriteInitRequest{frame_}; break;
        case Specifier::BlockWriteEnd       : s = BlockWriteEndRequest{frame_}; break;
        case Specifier::BlockReadInit       : s = BlockReadInitRequest{frame_}; break;
        case Specifier::BlockReadEnd        : s = BlockReadEndRequest{frame_}; break;
        case Specifier::BlockReadStart      : s = BlockReadStartRequest{frame_}; break;
        case Specifier::BlockReadAck        : s = BlockReadAckRequest{frame_}; break;
    };

    return s;
}

}} // namespace cc::can
