/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/BlockReply>
#include <cc/can/BlockWriteInitReply>
#include <cc/can/BlockWriteEndReply>
#include <cc/can/BlockWriteAckReply>
#include <cc/can/BlockReadInitReply>
#include <cc/can/BlockReadEndReply>

namespace cc {
namespace can {

String BlockReply::Instance::toString() const
{
    if (!isValid()) return String{};

    String s;
    switch (specifier()) {
        case Specifier::BlockWriteInit     : s = BlockWriteInitReply{frame_}; break;
        case Specifier::BlockWriteEnd      : s = BlockWriteEndReply{frame_}; break;
        case Specifier::BlockWriteAck      : s = BlockWriteAckReply{frame_}; break;
        case Specifier::BlockReadStreamInit:
        case Specifier::BlockReadInit      : s = BlockReadInitReply{frame_}; break;
        case Specifier::BlockReadEnd       : s = BlockReadEndReply{frame_}; break;
    };

    return s;
}

}} // namespace cc::can
