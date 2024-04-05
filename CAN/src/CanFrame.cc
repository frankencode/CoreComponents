/*
 * Copyright (C) 2019-2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/CanFrame>
#include <cc/Format>
#include <linux/can.h>
#include <cassert>
#include <cstring>
#include <cc/DEBUG>

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

    uint32_t canId() const { return can_id; }

    void setCanId(uint32_t newValue) { can_id = newValue; }

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

    String copyPayload() const
    {
        if (size() == 0) return String{};
        const int n = size();
        String other = String::allocate(n);
        for (int i = 0; i < n; ++i) {
            other[i] = data[i];
        }
        return other;
    }

    String copyPayload(int i, int j) const
    {
        if (size() == 0) return String{};
        const int n = size();
        String other = String::allocate(j - i + 1);
        if (j >= n) j = n - 1;
        for (int k = i; k <= j; ++k) {
            other[k - i] = data[k];
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

    void setError(bool on)
    {
        if (on) can_id |= CAN_ERR_FLAG;
        else    can_id &= ~CAN_ERR_FLAG;
    }

    bool isExtended() const { return can_id & CAN_EFF_FLAG; }

    void setExtended(bool on)
    {
        if (on) can_id |= CAN_EFF_FLAG;
        else    can_id &= ~CAN_EFF_FLAG;
    }

    bool equals(const State &other) const
    {
        if (canId() != other.canId()) return false;
        if (size() != other.size()) return false;
        const int n = size();
        for (int i = 0; i < n; ++i) {
            if (data[i] != other.data[i]) return false;
        }
        return true;
    }

    String toString(bool annotate) const
    {
        Format f;
        const int n = size();
        uint32_t canId = this->canId();
        if (isExtended()) canId &= (1u << 30) - 1;
        else canId &= (1u << 12) - 1;
        f << hex(canId, isExtended() ? 8 : 3) << " [" << n << "]";
        if (n > 0) f << " ";
        for (int i = 0; i < n; ++i) {
            f << hex(at(i), 2);
            if (i != n - 1) f << " ";
        }
        if (annotate) {
            f << " |";
            for (int i = 0; i < n; ++i) {
                uint8_t ch = at(i);
                if (ch < 0x20 || 0x7F <= ch) ch = '.';
                f << static_cast<char>(ch);
            }
            f << "|";
        }
        return f;
    }
};

CanFrame::CanFrame():
    Object{new State}
{}

CanFrame::CanFrame(uint32_t id):
    Object{new State{id}}
{}

bool CanFrame::read(const String &line, Out<CanFrame> frame, Out<uint64_t> deltaTime)
{
    uint32_t canId = 0;
    int dlc = 0;

    bool ok = true;
    long i0 = 0, i = 0;
    ok = ok && line.find(" [", &i);
    if (ok) {
        ok = ok && line.readNumberInRange<uint32_t, 16>(i0, i, &canId);
    }
    if (ok) {
        frame->canId(canId);
        i += 2;
        i0 = i;
        ok = ok && line.find("] ", &i);
    }
    if (ok) {
        ok = ok && line.readNumberInRange<int, 10>(i0, i, &dlc);
        ok = ok && (0 <= dlc && dlc <= 64);
    }
    if (ok) {
        frame->size(dlc);
        i += 2;
        i0 = i;
        for (int j = 0; j < dlc; ++j) {
            if (!line.find(' ', &i) && j != dlc - 1) {
                ok = false;
                break;
            }
            if (!line.readNumberInRange<uint8_t, 16>(i0, i, &frame->at(j))) {
                ok = false;
                break;
            }
            while (i < line.count() && line.at(i) == ' ') ++i;
            i0 = i;
        }
    }
    if (ok && deltaTime.requested()) {
        if (!line.readNumberInRange<uint64_t>(i, line.count(), &deltaTime)) {
            ok = false;
        }
    }
    return ok;
}

bool CanFrame::isValid() const
{
    return !Object::isNull() && me().isValid();
}

uint32_t CanFrame::canId() const
{
    return me().canId();
}

CanFrame &CanFrame::canId(uint32_t newValue)
{
    me().setCanId(newValue);
    return *this;
}

int CanFrame::size() const
{
    return me().size();
}

CanFrame &CanFrame::size(int n)
{
    me().resize(n);
    return *this;
}

uint8_t CanFrame::at(int i) const
{
    return me().at(i);
}

uint8_t &CanFrame::at(int i)
{
    return me().at(i);
}

String CanFrame::data() const
{
    return me().copyPayload();
}

String CanFrame::data(int i, int j) const
{
    assert(0 <= i && i <= j);
    return me().copyPayload(i, j);
}

void CanFrame::setData(const Bytes &data)
{
    me().setPayload(data);
}

bool CanFrame::rtr() const
{
    return me().isRemoteTransmissionRequest();
}

CanFrame &CanFrame::rtr(bool on)
{
    me().setRemoteTransmissionRequest(on);
    return *this;
}

bool CanFrame::err() const
{
    return me().isError();
}

CanFrame &CanFrame::err(bool on)
{
    me().setError(on);
    return *this;
}

bool CanFrame::eff() const
{
    return me().isExtended();
}

CanFrame &CanFrame::eff(bool on)
{
    me().setExtended(on);
    return *this;
}

bool CanFrame::operator==(const CanFrame &other) const
{
    return me().equals(other.me());
}

bool CanFrame::operator!=(const CanFrame &other) const
{
    return !(*this == other);
}

String CanFrame::toString(bool annotate) const
{
    return me().toString(annotate);
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
