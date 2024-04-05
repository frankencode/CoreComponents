/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/etp/protocol>
#include <cc/j1939/etp/Abort>
#include <cc/j1939/etp/ClearToSend>
#include <cc/j1939/etp/DataPacketOffset>
#include <cc/j1939/etp/DataPacket>
#include <cc/j1939/etp/EndOfMessage>
#include <cc/j1939/tp/protocol>
#include <cc/System>

namespace cc::j1939::etp {

void receive(CanMedia &media, const RequestToSend &request, Out<String> message, int timeout)
{
    if (message.requested() && message->count() != request.totalSize()) {
        message = String::allocate(request.totalSize());
    }
    uint32_t offset = 0;

    CanFrame frame;
    uint8_t nextPacketInWindow = 1;
    uint32_t nextPacket = 1;
    uint32_t clearedPacketCount = 0;
    uint32_t bytesToRead = request.totalSize();

    double nextTime = 0;
    if (timeout > 0) nextTime = System::now();

    while (bytesToRead > 0)
    {
        if (clearedPacketCount == 0) {
            CanFrame frame = ClearToSend::produce(request, nextPacket);
            media.write(frame);
            if (timeout > 0) nextTime += timeout * 0.001;
        }

        while (true)
        {
            if (timeout > 0 && !media.waitUntil(nextTime)) {
                media.write(Abort::produce(request, Abort::Reason::Timeout));
                throw Timeout{};
            }

            if (!media.read(&frame)) throw InputExhaustion{};

            DataPacketOffset dpo{frame};

            if (dpo && dpo.src() == request.src() && dpo.dst() == request.dst()) {
                nextPacketInWindow = 1;
                offset = dpo.packetOffset() * 7;
                clearedPacketCount = dpo.packetCount();
                break;
            }

            Abort abort{frame};
            if (abort && abort.src() == request.src() && abort.dst() == request.dst()) throw Aborted{};
        }

        if (timeout > 0) nextTime += timeout * 0.001;

        while (clearedPacketCount > 0 && bytesToRead > 0)
        {
            if (timeout > 0 && !media.waitUntil(nextTime)) {
                media.write(Abort::produce(request, Abort::Reason::Timeout));
                throw Timeout{};
            }

            if (!media.read(&frame)) throw InputExhaustion{};

            DataPacket packet{frame};
            if (packet && packet.src() == request.src() && packet.dst() == request.dst()) {
                if (packet.packetNumber() != nextPacketInWindow) {
                    media.write(Abort::produce(request, Abort::Reason::BadSequenceNumber));
                    throw PacketError{};
                }
                for (int i = 0; i < 7 && bytesToRead > 0; ++i) {
                    if (request.totalSize() <= offset) {
                        media.write(Abort::produce(request, Abort::Reason::BadPacketOffset));
                        throw PacketError{};
                    }
                    if (message.requested()) {
                        message->byteAt(offset) = packet.payloadAt(i);
                    }
                    ++offset;
                    --bytesToRead;
                }
                ++nextPacketInWindow;
                ++nextPacket;
                --clearedPacketCount;

                if (timeout > 0) nextTime += timeout * 0.001;
                continue;
            }

            Abort abort{frame};
            if (abort && abort.src() == request.src() && abort.dst() == request.dst()) throw Aborted{};
        }
    }

    if (bytesToRead == 0) {
        media.write(EndOfMessage::produce(request));
    }
}

void send(CanMedia &media, uint32_t pgn, uint8_t src, uint8_t dst, const Bytes &message, int timeout)
{
    assert(message.count() > 0);
    assert(message.count() <= 117440505);

    if (message.count() <= 1785) {
        tp::send(media, pgn, src, dst, message);
        return;
    }

    media.write(RequestToSend::produce(pgn, src, dst, message));

    const uint32_t packetCount = (message.count() + 6) / 7;
    uint32_t packetNumber = 1;

    double nextTime = 0;
    if (timeout > 0) nextTime = System::now() + timeout * 0.001;
    CanFrame frame;

    while (packetNumber <= packetCount)
    {
        if (timeout > 0 && !media.waitUntil(nextTime)) throw Timeout{};
        if (!media.read(&frame)) throw InputExhaustion{};

        Abort abort{frame};
        if (abort && abort.src() == dst && abort.dst() == src) throw Aborted{};

        ClearToSend cts{frame};
        if (!cts || cts.src() != dst || cts.dst() != src) continue;

        packetNumber = cts.nextPacketNumber();

        if (packetCount < packetNumber) break;

        const uint32_t packetOffset = (packetNumber - 1);
        uint32_t windowPacketCount = packetCount - packetNumber + 1;
        if (windowPacketCount > 0xFF) windowPacketCount = 0xFF;
        if (windowPacketCount > cts.clearedPacketCount()) windowPacketCount = cts.clearedPacketCount();

        media.write(DataPacketOffset::produce(pgn, windowPacketCount, packetOffset, src, dst));

        for (uint32_t windowPacketNumber = 1; windowPacketNumber <= windowPacketCount; ++windowPacketNumber)
        {
            media.write(DataPacket::produce(message, packetOffset, windowPacketNumber, src, dst));
        }

        packetNumber += windowPacketCount;

        if (timeout > 0) nextTime = System::now() + timeout * 0.001;
    }

    if (packetNumber == packetCount)
    {
        while (true) {
            if (timeout > 0 && !media.wait(nextTime)) throw Timeout{};
            if (!media.read(&frame)) throw InputExhaustion{};

            Abort abort{frame};
            if (abort && abort.src() == dst && abort.dst() == src) throw Aborted{};

            EndOfMessage ack{frame};
            if (!ack || ack.src() != dst || ack.dst() != src) continue;

            if (ack.totalSize() != message.count()) throw ClosingNegative{};
        }
    }
}

} // namespace cc::j1939::etp
