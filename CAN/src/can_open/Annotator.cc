 /*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/Annotator>
#include <cc/can_open/sdo/Request>
#include <cc/can_open/sdo/Reply>
#include <cc/can_open/sdo/AbortRequest>
#include <cc/can_open/sdo/AbortReply>
#include <cc/can_open/sdo/BlockReadStartRequest>
#include <cc/can_open/sdo/BlockWriteInitReply>
#include <cc/can_open/sdo/BlockWriteSegmentRequest>
#include <cc/can_open/sdo/BlockReadSegmentReply>
#include <cc/can_open/NetworkCommand>
#include <cc/can_open/Emergency>
#include <cc/can_open/HeartBeat>
#include <cc/Set>

namespace cc::can_open {

struct Annotator::State final: public CanAnnotator::State
{
    String annotate(const CanFrame &frame) override
    {
        String info;

        if (sdo::Reply{frame})
        {
            if (sdo::BlockWriteInitReply{frame}) {
                blockWriteTransfers.insert(sdo::BlockWriteInitReply{frame}.serverId());
            }
            else if (sdo::AbortReply{frame}) {
                blockWriteTransfers.remove(sdo::AbortReply{frame}.serverId());
            }

            if (blockReadTransfers.contains(sdo::Reply{frame}.serverId())) {
                if (sdo::BlockReadSegmentReply{frame}) {
                    if (sdo::BlockReadSegmentReply{frame}.isLast()) {
                        blockReadTransfers.remove(sdo::Reply{frame}.serverId());
                    }
                    info = sdo::BlockReadSegmentReply{frame}.toString();
                }
            }
        }
        else if (sdo::Request{frame})
        {
            if (sdo::BlockReadStartRequest{frame}) {
                blockReadTransfers.insert(sdo::BlockReadStartRequest{frame}.serverId());
            }
            else if (sdo::AbortRequest{frame}) {
                blockReadTransfers.remove(sdo::AbortRequest{frame}.serverId());
            }

            if (blockWriteTransfers.contains(sdo::Request{frame}.serverId())) {
                if (sdo::BlockWriteSegmentRequest{frame}) {
                    if (sdo::BlockWriteSegmentRequest{frame}.isLast()) {
                        blockWriteTransfers.remove(sdo::Request{frame}.serverId());
                    }
                    info = sdo::BlockWriteSegmentRequest{frame}.toString();
                }
            }
        }

        if (info == "") {
            if (sdo::Request{frame}) info = sdo::Request{frame}.toString();
            else if (sdo::Reply{frame}) info = sdo::Reply{frame}.toString();
            else if (HeartBeat{frame}) info = HeartBeat{frame}.toString();
            else if (Emergency{frame}) info = Emergency{frame}.toString();
            else if (NetworkCommand{frame}) info = NetworkCommand{frame}.toString();
        }

        return info;
    }

    Set<int> blockWriteTransfers;
    Set<int> blockReadTransfers;
};

Annotator::Annotator():
    CanAnnotator{new State}
{}

} // namespace cc::can_open
