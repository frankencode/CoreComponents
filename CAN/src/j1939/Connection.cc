/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/Connection>
#include <cc/BitArray>

namespace cc::j1939 {

struct Connection::State final: public Object::State
{
    State(uint32_t pgn, uint8_t src, uint8_t dst, uint32_t totalSize):
        pgn_{pgn},
        src_{src},
        dst_{dst},
        packetOffset_{0},
        message_{String::allocate(totalSize)},
        received_{(totalSize + 6) / 7}
    {}

    uint16_t id() const
    {
        return Connection::id(src_, dst_);
    }

    bool deposit(uint8_t packetNumber, const CanFrame &packet)
    {
        if (packetNumber == 0) {
            return false;
        }
        long packetIndex = packetOffset_ + static_cast<long>(packetNumber) - 1;
        if (packetIndex < 0 || received_.count() <= packetIndex) {
            return false;
        }
        received_[packetIndex] = true;
        long offset = packetIndex * 7;
        for (int i = 1; i < 8; ++i) {
            if (message_.count() <= offset) return false;
            message_[offset] = packet[i];
            ++offset;
        }
        return true;
    }

    bool collect(Out<String> message) const
    {
        for (bool ok: received_) {
            if (!ok) return false;
        }
        message = message_;
        return true;
    }

    uint32_t pgn_;
    uint8_t src_;
    uint8_t dst_;
    long packetOffset_;
    String message_;
    BitArray received_;
};

Connection::Connection(uint32_t pgn, uint8_t src, uint8_t dst, uint32_t totalSize):
    Object{new State{pgn, src, dst, totalSize}}
{}

uint32_t Connection::pgn() const
{
    return me().pgn_;
}

uint8_t Connection::src() const
{
    return me().src_;
}

uint8_t Connection::dst() const
{
    return me().dst_;
}

uint32_t Connection::totalSize() const
{
    return me().message_.count();
}

uint32_t Connection::packetCount() const
{
    return me().received_.count();
}

void Connection::setPacketOffset(uint32_t packetNumber)
{
    me().packetOffset_ = packetNumber;
}

bool Connection::deposit(uint8_t packetNumber, const CanFrame &packet)
{
    return me().deposit(packetNumber, packet);
}

bool Connection::collect(Out<String> message) const
{
    return me().collect(&message);
}

Connection::Id Connection::id(uint8_t src, uint8_t dst)
{
    return (static_cast<uint16_t>(src) << 8) | static_cast<uint16_t>(dst);
}

Connection::Id Connection::id() const
{
    return me().id();
}

std::strong_ordering Connection::operator<=>(const Connection &other) const
{
    return id() <=> other.id();
}

Connection::State &Connection::me()
{
    return Object::me.as<State>();
}

const Connection::State &Connection::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::j1939
