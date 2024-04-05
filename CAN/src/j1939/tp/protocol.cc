/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/tp/protocol>
#include <cc/j1939/tp/DataPacket>
#include <cc/j1939/tp/ClearToSend>
#include <cc/j1939/tp/Abort>
#include <cc/j1939/tp/EndOfMessage>
#include <cc/System>
#include <cc/Thread>

namespace cc::j1939::tp {

void receiveBroadcast(CanMedia &media, const BroadcastAnnounce &bam, Out<String> message, int timeout)
{
    if (bam.totalSize() > 0)
    {
        if (message.requested() && message->count() != bam.totalSize()) {
            message = String::allocate(bam.totalSize());
        }
        long offset = 0;

        CanFrame frame;
        uint8_t nextPacket = 1;

        double nextTime = 0;
        if (timeout > 0) nextTime = System::now() + timeout * 0.001;

        for (uint16_t bytesToRead = bam.totalSize(); bytesToRead > 0;)
        {
            if (timeout > 0 && !media.waitUntil(nextTime)) throw Timeout{};
            if (!media.read(&frame)) throw InputExhaustion{};
            DataPacket packet{frame};
            if (!packet || packet.src() != bam.src()) {
                continue;
            }
            if (packet.packetNumber() != nextPacket) {
                throw PacketError{};
            }
            for (int i = 0; i < 7 && bytesToRead > 0; ++i) {
                if (message.requested()) {
                    message->byteAt(offset) = packet.payloadAt(i);
                }
                ++offset;
                --bytesToRead;
            }
            ++nextPacket;
            if (timeout > 0) nextTime += timeout * 0.001;
        }
    }
}

void broadcast(CanMedia &media, uint32_t pgn, uint8_t src, const Bytes &message, int delay)
{
    assert(message.count() > 0);

    if (message.count() <= 8)
    {
        CanFrame frame{
            CanId{}
            .prio(7)
            .pgn(pgn)
            .src(src)
        };

        for (int i = 0; i < 8; ++i) {
            frame[i] = message.has(i) ? message(i) : 0xFF;
        }

        media.write(frame);
        return;
    }

    media.write(BroadcastAnnounce::produce(pgn, message, src));

    const uint8_t packetCount = (message.count() + 6) / 7;
    double nextTime = 0;
    if (delay > 0) {
        nextTime = System::now() + delay * 0.001;
        Thread::sleepUntil(nextTime);
    }

    for (uint8_t packetNumber = 1; packetNumber <= packetCount; ++packetNumber)
    {
        CanFrame frame = DataPacket::produce(message, packetNumber, src);
        media.write(frame);

        if (packetNumber <= packetCount && delay > 0) {
            Thread::sleepUntil(nextTime);
            nextTime += delay * 0.001;
        }
    }
}

void receive(CanMedia &media, const RequestToSend &request, Out<String> message, int timeout)
{
    if (message.requested() && message->count() != request.totalSize()) {
        message = String::allocate(request.totalSize());
    }
    long offset = 0;

    CanFrame frame;
    uint8_t nextPacket = 1;
    uint8_t clearedPacketCount = 0;
    uint8_t totalPacketCount = 0;
    uint16_t bytesToRead = request.totalSize();

    double nextTime = 0;
    if (timeout > 0) nextTime = System::now();

    while (bytesToRead > 0)
    {
        if (clearedPacketCount == 0) {
            CanFrame frame = ClearToSend::produce(request, nextPacket);
            media.write(frame);
            clearedPacketCount = ClearToSend{frame}.clearedPacketCount();
            if (timeout > 0) nextTime += timeout * 0.001;
        }

        if (timeout > 0 && !media.waitUntil(nextTime)) {
            media.write(Abort::produce(request, Abort::Reason::Timeout));
            throw Timeout{};
        }

        if (!media.read(&frame)) throw InputExhaustion{};

        Abort abort{frame};
        if (abort) {
            if (abort.src() == request.src() && abort.dst() == request.dst()) throw Aborted{};
            continue;
        }

        DataPacket packet{frame};
        if (!packet) continue;
        if (packet.src() != request.src() || packet.dst() != request.dst()) continue;
        if (packet.packetNumber() != nextPacket) throw PacketError{};

        for (int i = 0; i < 7 && bytesToRead > 0; ++i) {
            if (message.requested()) {
                message->byteAt(offset) = packet.payloadAt(i);
            }
            ++offset;
            --bytesToRead;
        }
        ++nextPacket;
        --clearedPacketCount;
        ++totalPacketCount;

        if (timeout > 0) nextTime += timeout * 0.001;
    }

    if (bytesToRead == 0) {
        media.write(EndOfMessage::produce(request, totalPacketCount));
    }
}

void send(CanMedia &media, uint32_t pgn, uint8_t src, uint8_t dst, const Bytes &message, int timeout)
{
    assert(message.count() > 0);
    assert(message.count() <= 1785);

    if (message.count() <= 8)
    {
        assert((pgn & 0xFFu) == 0 || (pgn & 0xFFu) == dst);

        CanFrame frame {
            CanId{}
            .prio(7)
            .pf(pgn >> 8)
            .dst(dst)
            .src(src)
        };

        for (int i = 0; i < 8; ++i) {
            frame[i] = message.has(i) ? message(i) : 0xFF;
        }

        media.write(frame);
        return;
    }

    media.write(RequestToSend::produce(pgn, src, dst, message));

    const uint16_t packetCount = (message.count() + 6) / 7;
    uint16_t packetNumber = 1;

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

        for (
            uint8_t clearedPacketCount = cts.clearedPacketCount();
            packetNumber <= packetCount && clearedPacketCount > 0;
            ++packetNumber, --clearedPacketCount
        ) {
            media.write(DataPacket::produce(message, packetNumber, src, dst));
        }

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

            if (ack.totalSize() != message.count() || ack.packetCount() != packetCount) throw ClosingNegative{};
        }
    }
}

String Error::message() const
{
    return "J1939.TP data transfer error";
}

String Timeout::message() const
{
    return "J1939.TP connection timeout";
}

String InputExhaustion::message() const
{
    return "J1939.TP input exhaustion";
}

String PacketError::message() const
{
    return "J1939.TP packet error";
}

String Aborted::message() const
{
    return "J1939.TP connection aborted";
}

String ClosingNegative::message() const
{
    return "J1939.TP closing negative";
}

} // namespace cc::j1939::tp
