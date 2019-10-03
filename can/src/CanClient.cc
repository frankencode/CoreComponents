/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanClient>
#include <cc/can/ReadRequest>
#include <cc/can/ReadReply>
#include <cc/can/ReadSegmentRequest>
#include <cc/can/ReadSegmentReply>
#include <cc/can/WriteRequest>
#include <cc/can/WriteReply>
#include <cc/can/WriteSegmentRequest>
#include <cc/can/WriteSegmentReply>
#include <cc/can/BlockWriteInitRequest>
#include <cc/can/BlockWriteInitReply>
#include <cc/can/BlockWriteSegmentRequest>
#include <cc/can/BlockWriteAckReply>
#include <cc/can/BlockWriteEndRequest>
#include <cc/can/BlockWriteEndReply>
#include <cc/can/BlockReadInitRequest>
#include <cc/can/BlockReadInitReply>
#include <cc/can/BlockReadStartRequest>
#include <cc/can/BlockReadSegmentReply>
#include <cc/can/BlockReadAckRequest>
#include <cc/can/BlockReadEndRequest>
#include <cc/can/BlockReadEndReply>
#include <cc/can/Crc16Sink>
#include <cc/can/AbortRequest>
#include <cc/can/AbortReply>
#include <cc/can/exceptions>

namespace cc {
namespace can {

Ref<CanClient> CanClient::create(CanMedia *media, int nodeId, int timeout)
{
    return new CanClient(media, nodeId, timeout);
}

CanClient::CanClient(CanMedia *media, int nodeId, int timeout):
    media_{media},
    nodeId_{nodeId},
    timeout_{timeout}
{}

String CanClient::read(int peerId, Selector selector)
{
    media_->writeFrame(
        ReadRequest::createFrame(peerId, selector)
    );

    while (true) {
        if (!media_->waitFrame(timeout_)) {
            canAbort(peerId, selector, Abort::Reason::ServiceDataTimeout);
            throw CanTimeout{};
        }

        auto frame = CanFrame::create();
        if (!media_->readFrame(frame)) throw CanDisconnect{};

        if (!ServiceReply{frame}) continue;
        if (ServiceReply{frame}->serverId() != peerId) continue;

        if (ReadReply{frame}) {
            if (ReadReply{frame}->selector() != selector)
                throw CanUnexpected{};

            if (ReadReply{frame}->hasExpeditedData())
                return ReadReply{frame}->expeditedData();

            break;
        }

        if (AbortReply{frame})
            throw CanPeerAbort{AbortReply{frame}->reason()};

        throw CanUnexpected{};
    }

    return segmentedReadTransfer(peerId, selector);
}

void CanClient::write(int peerId, Selector selector, const String &data, int switchThreshold)
{
    if (data->count() <= 4)
        expeditedWrite(peerId, selector, data);
    else if (data->count() <= switchThreshold)
        segmentedWrite(peerId, selector, data);
    else
        blockWrite(peerId, selector, data);
}

void CanClient::expeditedWrite(int peerId, Selector selector, const String &data)
{
    CC_ASSERT(0 < data->count() && data->count() <= 4);

    media_->writeFrame(
        WriteRequest::createFrame(peerId, selector, data)
    );

    while (true)
    {
        if (!media_->waitFrame(timeout_)) {
            canAbort(peerId, selector, Abort::Reason::ServiceDataTimeout);
            throw CanTimeout{};
        }

        auto frame = CanFrame::create();
        if (!media_->readFrame(frame)) throw CanDisconnect{};

        if (ServiceReply{frame}->serverId() != peerId) continue;

        if (WriteReply{frame}) {
            if (WriteReply{frame}->selector() != selector)
                throw CanUnexpected{};

            break;
        }

        if (AbortReply{frame})
            throw CanPeerAbort{AbortReply{frame}->reason()};
    }
}

String CanClient::segmentedReadTransfer(int peerId, Selector selector)
{
    auto segments = StringList::create();

    while (true) {
        auto requestFrame = ReadSegmentRequest::createFrame(peerId, segments->count() & 1);
        auto request = ReadSegmentRequest{requestFrame};
        media_->writeFrame(requestFrame);

        Ref<CanFrame> frame;

        while (true) {
            if (!media_->waitFrame(timeout_)) {
                canAbort(peerId, selector, Abort::Reason::ServiceDataTimeout);
                throw CanTimeout{};
            }

            frame = CanFrame::create();
            if (!media_->readFrame(frame)) throw CanDisconnect{};

            if (ServiceReply{frame}->serverId() == peerId) break;
        }

        if (AbortReply{frame})
            throw CanPeerAbort{AbortReply{frame}->reason()};

        if (!ReadSegmentReply{frame})
            throw CanUnexpected{};

        if (ReadSegmentReply{frame}->toggle() != request->toggle()) {
            canAbort(peerId, selector, Abort::Reason::ToggleBitInvalid);
            throw CanLocalAbort{Abort::Reason::ToggleBitInvalid};
        }

        segments->append(ReadSegmentReply{frame}->data());

        if (ReadSegmentReply{frame}->isLast()) break;
    }

    return segments->join();
}

void CanClient::segmentedWrite(int peerId, Selector selector, const String &data)
{
    CC_ASSERT(4 < data->count());

    media_->writeFrame(
        WriteRequest::createFrame(peerId, selector, data)
    );

    while (true)
    {
        if (!media_->waitFrame(timeout_)) {
            canAbort(peerId, selector, Abort::Reason::ServiceDataTimeout);
            throw CanTimeout{};
        }

        auto frame = CanFrame::create();
        if (!media_->readFrame(frame)) throw CanDisconnect{};

        if (ServiceReply{frame}->serverId() != peerId) continue;

        if (WriteReply{frame}) {
            if (WriteReply{frame}->selector() != selector)
                throw CanUnexpected{};

            break;
        }

        if (AbortReply{frame})
            throw CanPeerAbort{AbortReply{frame}->reason()};
    }

    bool toggle = false;

    for (int segmentStart = 0; segmentStart < data->count(); segmentStart += 7)
    {
        media_->writeFrame(
            WriteSegmentRequest::createFrame(peerId, data, segmentStart)
        );

        while (true)
        {
            if (!media_->waitFrame(timeout_)) {
                canAbort(peerId, selector, Abort::Reason::ServiceDataTimeout);
                throw CanTimeout{};
            }

            auto frame = CanFrame::create();
            if (!media_->readFrame(frame)) throw CanDisconnect{};

            if (!ServiceReply{frame}) continue;
            if (ServiceReply{frame}->serverId() != peerId) continue;

            if (AbortReply{frame})
                throw CanPeerAbort{AbortReply{frame}->reason()};

            if (!WriteSegmentReply{frame})
                throw CanUnexpected{};

            if (WriteSegmentReply{frame}->toggle() != toggle) {
                canAbort(peerId, selector, Abort::Reason::ToggleBitInvalid);
                throw CanLocalAbort{Abort::Reason::ToggleBitInvalid};
            }

            break;
        }

        toggle = !toggle;
    }
}

String CanClient::blockRead(int peerId, Selector selector, int blockSize, int switchThreshold)
{
    bool crcSupport = false;
    std::function<String()> fallback;

    blockReadInitiate(peerId, selector, blockSize, switchThreshold, &crcSupport, &fallback);

    if (fallback) return fallback();

    String data = blockReadTransfer(peerId, selector, blockSize);

    blockReadEnd(peerId, selector, &data, crcSupport);

    return data;
}

void CanClient::blockReadInitiate(int peerId, Selector selector, int blockSize, int switchThreshold, bool *crcSupport, std::function<String()> *fallback)
{
    CC_ASSERT(0 < blockSize && blockSize <= 0x7F);
    CC_ASSERT(0 <= switchThreshold);

    media_->writeFrame(
        BlockReadInitRequest::createFrame(
            peerId, selector, blockSize, switchThreshold
        )
    );

    auto frame = getNextReply(peerId, selector);

    if (AbortReply{frame})
        throw CanPeerAbort{AbortReply{frame}->reason()};

    if (ReadReply{frame}) {
        *fallback = [=](){
            if (ReadReply{frame}->selector() != selector)
                throw CanUnexpected{};

            if (ReadReply{frame}->hasExpeditedData())
                return ReadReply{frame}->expeditedData();

            return segmentedReadTransfer(peerId, selector);
        };

        return;
    }

    if (!BlockReadInitReply{frame}) {
        canAbort(peerId, selector, Abort::Reason::GeneralError);
        throw CanAbort{Abort::Reason::GeneralError};
    }

    if (BlockReadInitReply{frame}->selector() != selector) {
        canAbort(peerId, BlockReadInitReply{frame}->selector(), Abort::Reason::GeneralError);
        throw CanAbort{Abort::Reason::GeneralError};
    }

    *crcSupport = BlockReadInitReply{frame}->crcSupport();
}

String CanClient::blockReadTransfer(int peerId, Selector selector, int blockSize)
{
    media_->writeFrame(BlockReadStartRequest::createFrame(peerId));

    int seqNum = 1;

    auto parts = StringList::create();

    for (bool go = true; go;)
    {
        auto segments = StringList::create();

        for (; seqNum <= blockSize; ++seqNum)
        {
            auto frame = getNextReply(peerId, selector);

            if (!BlockReadSegmentReply{frame}) {
                canAbort(peerId, selector, Abort::Reason::GeneralError);
                throw CanAbort{Abort::Reason::GeneralError};
            }

            if (BlockReadSegmentReply{frame}->sequenceNumber() != seqNum)
                break;

            segments->append(BlockReadSegmentReply{frame}->segmentData());
            if (BlockReadSegmentReply{frame}->isLast()) {
                go = false;
                break;
            }
        }

        seqNum -= go;

        media_->writeFrame(
            BlockReadAckRequest::createFrame(peerId, seqNum, blockSize)
        );

        if (seqNum == blockSize || !go) {
            parts->append(segments->join());
            seqNum = 1;
        }
    }

    return parts->join();
}

void CanClient::blockReadEnd(int peerId, Selector selector, String *data, bool crcSupport)
{
    auto frame = getNextReply(peerId, selector);

    if (AbortReply{frame})
        throw CanPeerAbort{AbortReply{frame}->reason()};

    if (!BlockReadEndReply{frame}) {
        canAbort(peerId, selector, Abort::Reason::GeneralError);
        throw CanUnexpected{};
    }

    int lastSegmentDataCount = BlockReadEndReply{frame}->lastSegmentDataCount();
    uint16_t crcReceived = BlockReadEndReply{frame}->crc();

    if (lastSegmentDataCount < 7) {
        const int overhang = 7 - lastSegmentDataCount;
        mutate(*data)->truncate((*data)->count() - overhang);
    }

    if (crcSupport) {
        uint16_t crcComputed = crc16(*data);
        if (crcReceived != crcComputed) {
            canAbort(peerId, selector, Abort::Reason::CrcError);
            throw CanAbort{Abort::Reason::CrcError};
        }
    }

    media_->writeFrame(
        BlockReadEndRequest::createFrame(peerId)
    );
}

void CanClient::blockWrite(int peerId, Selector selector, const String &data)
{
    int blockSize = 0;
    bool crcSupport = false;

    blockWriteInitiate(peerId, selector, data, &blockSize, &crcSupport);

    for (int offset = 0; offset < data->count();)
        blockWriteTransfer(peerId, selector, data, &blockSize, &offset);

    blockWriteEnd(peerId, selector, data, crcSupport);
}

void CanClient::blockWriteInitiate(int peerId, Selector selector, const String &data, int *blockSize, bool *crcSupport)
{
    media_->writeFrame(
        BlockWriteInitRequest::createFrame(peerId, selector, data)
    );

    auto frame = getNextReply(peerId, selector);

    if (AbortReply{frame})
        throw CanPeerAbort{AbortReply{frame}->reason()};

    if (!BlockWriteInitReply{frame}) {
        canAbort(peerId, selector, Abort::Reason::GeneralError);
        throw CanAbort{Abort::Reason::GeneralError};
    }

    if (BlockWriteInitReply{frame}->selector() != selector) {
        canAbort(peerId, BlockWriteInitReply{frame}->selector(), Abort::Reason::SelectorInvalid);
        throw CanAbort{Abort::Reason::SelectorInvalid};
    }

    if (BlockWriteInitReply{frame}->blockSize() == 0) {
        canAbort(peerId, selector, Abort::Reason::BlockSizeInvalid);
        throw CanAbort{Abort::Reason::BlockSizeInvalid};
    }

    *blockSize = BlockWriteInitReply{frame}->blockSize();
    if (*blockSize > data->count() * 7) *blockSize = data->count() / 7 + (data->count() % 7 > 0);
    *crcSupport = BlockWriteInitReply{frame}->crcSupport();
}

void CanClient::blockWriteTransfer(int peerId, Selector selector, const String &data, int *blockSize, int *offset)
{
    int seqNum = 0;

    while (seqNum < *blockSize)
    {
        for (; seqNum < *blockSize; ++seqNum) {
            media_->writeFrame(
                 BlockWriteSegmentRequest::createFrame(peerId, data, *offset + 7 * seqNum, seqNum + 1)
            );
        }

        auto frame = getNextReply(peerId, selector);

        if (AbortReply{frame})
            throw CanPeerAbort{AbortReply{frame}->reason()};

        if (!BlockWriteAckReply{frame}) {
            canAbort(peerId, selector, Abort::Reason::GeneralError);
            throw CanAbort{Abort::Reason::GeneralError};
        }

        seqNum = BlockWriteAckReply{frame}->sequenceNumber();
        *blockSize = BlockWriteAckReply{frame}->newBlockSize();
    }

    *offset += 7 * seqNum;

    const int remaining = data->count() - *offset;
    if (*blockSize * 7 > remaining)
        *blockSize = remaining / 7 + (remaining % 7 > 0);
}

void CanClient::blockWriteEnd(int peerId, Selector selector, const String &data, bool crcSupport)
{
    media_->writeFrame(
        BlockWriteEndRequest::createFrame(peerId, data, crcSupport)
    );

    auto frame = getNextReply(peerId, selector);

    if (AbortReply{frame})
        throw CanPeerAbort{AbortReply{frame}->reason()};

    if (!BlockWriteEndReply{frame}) {
        canAbort(peerId, selector, Abort::Reason::GeneralError);
        throw CanAbort{Abort::Reason::GeneralError};
    }
}

Ref<CanFrame> CanClient::getNextReply(int peerId, Selector selector)
{
    auto frame = CanFrame::create();

    while (true)
    {
        if (!media_->waitFrame(timeout_)) {
            canAbort(peerId, selector, Abort::Reason::ServiceDataTimeout);
            throw CanTimeout{};
        }

        if (!media_->readFrame(frame))
            throw CanDisconnect{};

        if (!ServiceReply{frame}) continue;
        if (ServiceReply{frame}->serverId() != peerId) continue;

        break;
    }

    return frame;
}

void CanClient::canAbort(int peerId, Selector selector, Abort::Reason reason)
{
    media_->writeFrame(
        AbortRequest::createFrame(peerId, selector, reason)
    );
}

}} // namespace cc::can
