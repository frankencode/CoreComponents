/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/sdo/protocol>
#include <cc/can_open/sdo/ReadRequest>
#include <cc/can_open/sdo/ReadReply>
#include <cc/can_open/sdo/ReadSegmentRequest>
#include <cc/can_open/sdo/ReadSegmentReply>
#include <cc/can_open/sdo/WriteRequest>
#include <cc/can_open/sdo/WriteReply>
#include <cc/can_open/sdo/WriteSegmentRequest>
#include <cc/can_open/sdo/WriteSegmentReply>
#include <cc/can_open/sdo/BlockWriteInitRequest>
#include <cc/can_open/sdo/BlockWriteInitReply>
#include <cc/can_open/sdo/BlockWriteSegmentRequest>
#include <cc/can_open/sdo/BlockWriteAckReply>
#include <cc/can_open/sdo/BlockWriteEndRequest>
#include <cc/can_open/sdo/BlockWriteEndReply>
#include <cc/can_open/sdo/BlockReadInitRequest>
#include <cc/can_open/sdo/BlockReadInitReply>
#include <cc/can_open/sdo/BlockReadStartRequest>
#include <cc/can_open/sdo/BlockReadSegmentReply>
#include <cc/can_open/sdo/BlockReadAckRequest>
#include <cc/can_open/sdo/BlockReadEndRequest>
#include <cc/can_open/sdo/BlockReadEndReply>
#include <cc/can_open/sdo/AbortRequest>
#include <cc/can_open/sdo/AbortReply>
#include <cc/can_open/Crc16Sink>
#include <cc/Format>
#include <cassert>

namespace cc::can_open::sdo {

Bytes receiveSegmented(CanMedia &media, int peerId, Selector selector, int timeout)
{
    List<Bytes> segments;

    while (true) {
        CanFrame requestFrame = ReadSegmentRequest::createFrame(peerId, segments.count() & 1);
        ReadSegmentRequest request{requestFrame};
        media.write(requestFrame);

        CanFrame frame;

        while (true) {
            if (!media.wait(timeout)) {
                media.write(
                    AbortRequest::createFrame(peerId, selector, ErrorCode::ServiceDataTimeout)
                );
                throw Timeout{};
            }

            if (!media.read(&frame)) throw InputExhaustion{};

            if (Reply{frame}.serverId() == peerId) break;
        }

        if (AbortReply{frame}) {
            throw RemotelyAborted{AbortReply{frame}.error()};
        }

        if (!ReadSegmentReply{frame}) {
            throw Unexpected{};
        }

        if (ReadSegmentReply{frame}.toggle() != request.toggle()) {
            media.write(
                AbortRequest::createFrame(peerId, selector, ErrorCode::ToggleBitInvalid)
            );
            throw LocallyAborted{ErrorCode::ToggleBitInvalid};
        }

        segments.append(ReadSegmentReply{frame}.data());

        if (ReadSegmentReply{frame}.isLast()) break;
    }

    return segments.join();
}

void writeExpedited(CanMedia &media, int peerId, Selector selector, const Bytes &data, int timeout)
{
    assert(0 < data.count() && data.count() <= 4);

    media.write(
        WriteRequest::createFrame(peerId, selector, data)
    );

    while (true)
    {
        if (!media.wait(timeout)) {
            media.write(
                AbortRequest::createFrame(peerId, selector, ErrorCode::ServiceDataTimeout)
            );
            throw Timeout{};
        }

        CanFrame frame;
        if (!media.read(&frame)) throw InputExhaustion{};

        if (Reply{frame}.serverId() != peerId) continue;

        if (WriteReply{frame}) {
            if (WriteReply{frame}.selector() != selector) {
                throw Unexpected{};
            }

            break;
        }

        if (AbortReply{frame}) {
            throw RemotelyAborted{AbortReply{frame}.error()};
        }
    }
}

void writeSegmented(CanMedia &media, int peerId, Selector selector, const Bytes &data, int timeout)
{
    assert(4 < data.count());

    media.write(
        WriteRequest::createFrame(peerId, selector, data)
    );

    while (true)
    {
        if (!media.wait(timeout)) {
            media.write(
                AbortRequest::createFrame(peerId, selector, ErrorCode::ServiceDataTimeout)
            );
            throw Timeout{};
        }

        CanFrame frame;
        if (!media.read(&frame)) throw InputExhaustion{};

        if (Reply{frame}.serverId() != peerId) continue;

        if (WriteReply{frame}) {
            if (WriteReply{frame}.selector() != selector) {
                throw Unexpected{};
            }

            break;
        }

        if (AbortReply{frame}) {
            throw RemotelyAborted{AbortReply{frame}.error()};
        }
    }

    bool toggle = false;

    for (int segmentStart = 0; segmentStart < data.count(); segmentStart += 7)
    {
        media.write(
            WriteSegmentRequest::createFrame(peerId, data, segmentStart)
        );

        while (true)
        {
            if (!media.wait(timeout)) {
                media.write(
                    AbortRequest::createFrame(peerId, selector, ErrorCode::ServiceDataTimeout)
                );
                throw Timeout{};
            }

            CanFrame frame;
            if (!media.read(&frame)) throw InputExhaustion{};

            if (!Reply{frame}) continue;
            if (Reply{frame}.serverId() != peerId) continue;

            if (AbortReply{frame}) {
                throw RemotelyAborted{AbortReply{frame}.error()};
            }

            if (!WriteSegmentReply{frame}) {
                throw Unexpected{};
            }

            if (WriteSegmentReply{frame}.toggle() != toggle) {
                media.write(
                    AbortRequest::createFrame(peerId, selector, ErrorCode::ToggleBitInvalid)
                );
                throw LocallyAborted{ErrorCode::ToggleBitInvalid};
            }

            break;
        }

        toggle = !toggle;
    }
}

CanFrame getNextReply(CanMedia &media, int peerId, Selector selector, int timeout)
{
    CanFrame frame;

    while (true)
    {
        if (!media.wait(timeout)) {
            media.write(
                AbortRequest::createFrame(peerId, selector, ErrorCode::ServiceDataTimeout)
            );
            throw Timeout{};
        }

        if (!media.read(&frame)) {
            throw InputExhaustion{};
        }

        if (!Reply{frame}) continue;
        if (Reply{frame}.serverId() != peerId) continue;

        break;
    }

    return frame;
}

Bytes blockReadInitiate(CanMedia &media, int peerId, Selector selector, int blockSize, int switchThreshold, int timeout, Out<bool> fallback, Out<bool> crcSupport)
{
    assert(0 < blockSize && blockSize <= 0x7F);
    assert(0 <= switchThreshold);

    media.write(
        BlockReadInitRequest::createFrame(
            peerId, selector, blockSize, switchThreshold
        )
    );

    CanFrame frame = getNextReply(media, peerId, selector, timeout);

    if (AbortReply{frame}) {
        throw RemotelyAborted{AbortReply{frame}.error()};
    }

    if (ReadReply{frame}) {
        *fallback = true;

        if (ReadReply{frame}.selector() != selector) {
            throw Unexpected{};
        }

        if (ReadReply{frame}.hasExpeditedData()) {
            return ReadReply{frame}.expeditedData();
        }

        return receiveSegmented(media, peerId, selector, timeout);
    }

    if (!BlockReadInitReply{frame}) {
        media.write(
            AbortRequest::createFrame(peerId, selector, ErrorCode::GeneralError)
        );
        throw LocallyAborted{ErrorCode::GeneralError};
    }

    if (BlockReadInitReply{frame}.selector() != selector) {
        media.write(
            AbortRequest::createFrame(peerId, BlockReadInitReply{frame}.selector(), ErrorCode::GeneralError)
        );
        throw LocallyAborted{ErrorCode::GeneralError};
    }

    *fallback = false;
    *crcSupport = BlockReadInitReply{frame}.crcSupport();

    return Bytes{};
}

Bytes blockReadTransfer(CanMedia &media, int peerId, Selector selector, int blockSize, int timeout)
{
    media.write(BlockReadStartRequest::createFrame(peerId));

    int seqNum = 1;

    List<Bytes> parts;

    for (bool go = true; go;)
    {
        List<Bytes> segments;

        for (; seqNum <= blockSize; ++seqNum)
        {
            auto frame = getNextReply(media, peerId, selector, timeout);

            if (!BlockReadSegmentReply{frame}) {
                media.write(
                    AbortRequest::createFrame(peerId, selector, ErrorCode::GeneralError)
                );
                throw LocallyAborted{ErrorCode::GeneralError};
            }

            if (BlockReadSegmentReply{frame}.sequenceNumber() != seqNum) {
                break;
            }

            segments.append(BlockReadSegmentReply{frame}.segmentData());

            if (BlockReadSegmentReply{frame}.isLast()) {
                go = false;
                break;
            }
        }

        seqNum -= go;

        media.write(
            BlockReadAckRequest::createFrame(peerId, seqNum, blockSize)
        );

        if (seqNum == blockSize || !go) {
            parts.append(segments.join());
            seqNum = 1;
        }
    }

    return parts.join();
}

void blockReadEnd(CanMedia &media, int peerId, Selector selector, InOut<Bytes> data, bool crcSupport, int timeout)
{
    CanFrame frame = getNextReply(media, peerId, selector, timeout);

    if (AbortReply{frame}) {
        throw RemotelyAborted{AbortReply{frame}.error()};
    }

    if (!BlockReadEndReply{frame}) {
        media.write(
            AbortRequest::createFrame(peerId, selector, ErrorCode::GeneralError)
        );
        throw LocallyAborted{ErrorCode::GeneralError};
    }

    int lastSegmentDataCount = BlockReadEndReply{frame}.finalSegmentSize();
    uint16_t crcReceived = BlockReadEndReply{frame}.crc();

    if (lastSegmentDataCount < 7) {
        const int overhang = 7 - lastSegmentDataCount;
        data->truncate(data->count() - overhang);
    }

    if (crcSupport) {
        uint16_t crcComputed = crc16(*data);
        if (crcReceived != crcComputed) {
            media.write(
                AbortRequest::createFrame(peerId, selector, ErrorCode::CrcError)
            );
            throw LocallyAborted{ErrorCode::CrcError};
        }
    }

    media.write(
        BlockReadEndRequest::createFrame(peerId)
    );
}

void blockWriteInitiate(CanMedia &media, int peerId, Selector selector, const Bytes &data, int timeout, Out<int> blockSize, Out<bool> crcSupport)
{
    media.write(
        BlockWriteInitRequest::createFrame(peerId, selector, data)
    );

    CanFrame frame = getNextReply(media, peerId, selector, timeout);

    if (AbortReply{frame}) {
        throw RemotelyAborted{AbortReply{frame}.error()};
    }

    if (!BlockWriteInitReply{frame}) {
        media.write(
            AbortRequest::createFrame(peerId, selector, ErrorCode::GeneralError)
        );
        throw LocallyAborted{ErrorCode::GeneralError};
    }

    if (BlockWriteInitReply{frame}.selector() != selector) {
        media.write(
            AbortRequest::createFrame(peerId, BlockWriteInitReply{frame}.selector(), ErrorCode::SelectorInvalid)
        );
        throw LocallyAborted{ErrorCode::SelectorInvalid};
    }

    if (BlockWriteInitReply{frame}.blockSize() == 0) {
        media.write(
            AbortRequest::createFrame(peerId, selector, ErrorCode::BlockSizeInvalid)
        );
        throw LocallyAborted{ErrorCode::BlockSizeInvalid};
    }

    *blockSize = BlockWriteInitReply{frame}.blockSize();
    if (*blockSize > data.count() * 7) *blockSize = data.count() / 7 + (data.count() % 7 > 0);
    *crcSupport = BlockWriteInitReply{frame}.crcSupport();
}

void blockWriteTransfer(CanMedia &media, int peerId, Selector selector, const Bytes &data, int timeout, Out<int> blockSize, Out<int> offset)
{
    int seqNum = 0;

    while (seqNum < *blockSize)
    {
        for (; seqNum < *blockSize; ++seqNum) {
            media.write(
                BlockWriteSegmentRequest::createFrame(peerId, data, *offset + 7 * seqNum, seqNum + 1)
            );
        }

        CanFrame frame = getNextReply(media, peerId, selector, timeout);

        if (AbortReply{frame}) {
            throw RemotelyAborted{AbortReply{frame}.error()};
        }

        if (!BlockWriteAckReply{frame}) {
            media.write(
                AbortRequest::createFrame(peerId, selector, ErrorCode::GeneralError)
            );
            throw LocallyAborted{ErrorCode::GeneralError};
        }

        seqNum = BlockWriteAckReply{frame}.sequenceNumber();
        *blockSize = BlockWriteAckReply{frame}.newBlockSize();
    }

    *offset += 7 * seqNum;

    const int remaining = data.count() - *offset;
    if (*blockSize * 7 > remaining) {
        *blockSize = remaining / 7 + (remaining % 7 > 0);
    }
}

void blockWriteEnd(CanMedia &media, int peerId, Selector selector, const Bytes &data, int timeout, bool crcSupport)
{
    media.write(
        BlockWriteEndRequest::createFrame(peerId, data, crcSupport)
    );

    CanFrame frame = getNextReply(media, peerId, selector, timeout);

    if (AbortReply{frame}) {
        throw RemotelyAborted{AbortReply{frame}.error()};
    }

    if (!BlockWriteEndReply{frame}) {
        media.write(
            AbortRequest::createFrame(peerId, selector, ErrorCode::GeneralError)
        );
        throw LocallyAborted{ErrorCode::GeneralError};
    }
}

Bytes read(CanMedia &media, int peerId, Selector selector, int timeout)
{
    media.write(
        ReadRequest::createFrame(peerId, selector)
    );

    while (true) {
        if (!media.wait(timeout)) {
            media.write(
                AbortRequest::createFrame(peerId, selector, ErrorCode::ServiceDataTimeout)
            );
            throw Timeout{};
        }

        CanFrame frame;
        if (!media.read(&frame)) throw InputExhaustion{};

        if (!Reply{frame}) continue;
        if (Reply{frame}.serverId() != peerId) continue;

        if (ReadReply{frame}) {
            if (ReadReply{frame}.selector() != selector) {
                throw Unexpected{};
            }
            if (ReadReply{frame}.hasExpeditedData()) {
                return ReadReply{frame}.expeditedData();
            }
            break;
        }

        if (AbortReply{frame}) {
            throw RemotelyAborted{AbortReply{frame}.error()};
        }

        throw Unexpected{};
    }

    return receiveSegmented(media, peerId, selector, timeout);
}

void write(CanMedia &media, int peerId, Selector selector, const Bytes &data, int timeout)
{
    if (data.count() <= 4)
        writeExpedited(media, peerId, selector, data, timeout);
    else
        writeSegmented(media, peerId, selector, data, timeout);
}

Bytes blockRead(CanMedia &media, int peerId, Selector selector, int timeout, int blockSize, int switchThreshold)
{
    bool fallback = false;
    bool crcSupport = false;

    Bytes data = blockReadInitiate(media, peerId, selector, blockSize, switchThreshold, timeout, &fallback, &crcSupport);

    if (fallback) return data;

    data = blockReadTransfer(media, peerId, selector, blockSize, timeout);

    blockReadEnd(media, peerId, selector, &data, crcSupport, timeout);

    return data;
}

void blockWrite(CanMedia &media, int peerId, Selector selector, const Bytes &data, int timeout)
{
    int blockSize = 0;
    bool crcSupport = false;

    blockWriteInitiate(media, peerId, selector, data, timeout, &blockSize, &crcSupport);

    for (int offset = 0; offset < data.count();) {
        blockWriteTransfer(media, peerId, selector, data, timeout, &blockSize, &offset);
    }

    blockWriteEnd(media, peerId, selector, data, timeout, crcSupport);
}

String Aborted::message() const
{
    return Format{"Data transfer was aborted (0x%%: %%)"} << hex(+error()) << error() ;
}

String Timeout::message() const
{
    return "Timeout during data transfer";
}

String InputExhaustion::message() const
{
    return "Media lost during data transfer";
}

String Unexpected::message() const
{
    return "Unexpected frame encountered during data transfer";
}

} // namespace cc::can_open::sdo
