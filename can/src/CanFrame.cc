/*
 * Copyright (C) 2019-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/CanFrame>
#include <cc/Format>
#include <linux/can.h>
#include <cassert>
#include <cstring>

namespace cc {

using StructCanFrame = struct can_frame;

struct CanFrame::State: public Object::State, public StructCanFrame
{
    State()
    {
        std::memset(static_cast<StructCanFrame *>(this), 0, sizeof(StructCanFrame));
    }

    explicit State(uint32_t id):
        State{}
    {
        can_dlc = 8;
        can_id = id;
    }

    bool isValid() const { return can_id != 0 || can_dlc != 0; }

    uint32_t id() const { return can_id & CAN_EFF_MASK; }

    void setId(uint32_t newValue) { can_id = newValue; }

    int size() const { return can_dlc; }

    void resize(int n) {
        assert(0 <= n && n <= CAN_MAX_DLEN);
        can_dlc = n;
    }

    uint8_t at(int i) const
    {
        assert(0 <= i && i < can_dlc && i < CAN_MAX_DLEN);
        return data[i];
    }

    uint8_t &at(int i) {
        assert(0 <= i && i < can_dlc && i < CAN_MAX_DLEN);
        return data[i];
    }

    Bytes copyPayload() const
    {
        if (size() == 0) return Bytes{};
        const int n = size();
        Bytes other = Bytes::allocate(n);
        for (int i = 0; i < n; ++i) {
            other[i] = data[i];
        }
        return other;
    }

    void setPayload(const Bytes &other)
    {
        const int n = other.size();
        if (size() != n) resize(n);
        for (int i = 0; i < n; ++i) {
            data[i] = other[i];
        }
    }

    bool isRemoteTransmissionRequest() const { return can_id & CAN_RTR_FLAG; }

    void setRemoteTransmissionRequest(bool on)
    {
        if (on) can_id |= CAN_RTR_FLAG;
        else    can_id &= ~CAN_RTR_FLAG;
    }

    bool isError() const { return can_id & CAN_ERR_FLAG; }

    bool isExtended() const { return can_id & CAN_EFF_FLAG; }

    bool equals(const State &other) const
    {
        if (id() != other.id()) return false;
        if (size() != other.size()) return false;
        const int n = size();
        for (int i = 0; i < n; ++i) {
            if (data[i] != other.data[i]) return false;
        }
        return true;
    }

    String toString() const
    {
        Format f;
        const int n = size();
        f << hex(id(), 3) << " [" << n << "]";
        if (n > 0) f << " ";
        for (int i = 0; i < n; ++i) {
            f << hex(at(i), 2);
            if (i != n - 1) f << ".";
        }
        if (isRemoteTransmissionRequest()) f << " // RTR";
        return f;
    }
};

CanFrame::CanFrame():
    Object{new State}
{}

CanFrame::CanFrame(uint32_t id):
    Object{new State{id}}
{}

bool CanFrame::isValid() const
{
    return me().isValid();
}

uint32_t CanFrame::id() const
{
    return me().id();
}

void CanFrame::setId(uint32_t newValue)
{
    me().setId(newValue);
}

int CanFrame::size() const
{
    return me().size();
}

void CanFrame::resize(int n)
{
    me().resize(n);
}

uint8_t CanFrame::at(int i) const
{
    return me().at(i);
}

uint8_t &CanFrame::at(int i)
{
    return me().at(i);
}

Bytes CanFrame::data() const
{
    return me().copyPayload();
}

void CanFrame::setData(const Bytes &data)
{
    me().setPayload(data);
}

bool CanFrame::isRemoteTransmissionRequest() const
{
    return me().isRemoteTransmissionRequest();
}

void CanFrame::setRemoteTransmissionRequest(bool on)
{
    me().setRemoteTransmissionRequest(on);
}

bool CanFrame::isError() const
{
    return me().isError();
}

bool CanFrame::isExtended() const
{
    return me().isExtended();
}

bool CanFrame::operator==(const CanFrame &other) const
{
    return me().equals(other.me());
}

bool CanFrame::operator!=(const CanFrame &other) const
{
    return !(*this == other);
}

String CanFrame::toString() const
{
    return me().toString();
}

void *CanFrame::frame()
{
    return static_cast<StructCanFrame *>(&me());
}

const void *CanFrame::frame() const
{
    return static_cast<const StructCanFrame *>(&me());
}

size_t CanFrame::frameSize() const
{
    return sizeof(StructCanFrame);
}

CanFrame::State &CanFrame::me()
{
    return Object::me.as<State>();
}

const CanFrame::State &CanFrame::me() const
{
    return Object::me.as<State>();
}

String str(const CanFrame &frame)
{
    return frame.toString();
}

} // namespace cc
