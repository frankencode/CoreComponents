/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanFrame>
#include <cc/Format>
#include <string.h>

namespace cc {
namespace can {

CanFrame::Instance::Instance()
{
    ::memset(static_cast<StructCanFrame *>(this), 0, sizeof(StructCanFrame));
}

CanFrame::Instance::Instance(canid_t canId)
{
    ::memset(static_cast<StructCanFrame *>(this), 0, sizeof(StructCanFrame));
    can_dlc = 8;
    can_id = canId;
}

bool CanFrame::Instance::equals(const Instance *other) const
{
    if (canId() != other->canId()) return false;
    if (payloadCount() != other->payloadCount()) return false;
    for (int i = 0; i < payloadCount(); ++i) {
        if (payloadAt(i) != other->payloadAt(i))
            return false;
    }
    return true;
}

String CanFrame::Instance::payload() const
{
    if (payloadCount() == 0) return String{};
    String data = String::allocate(payloadCount());
    for (int i = 0; i < data->count(); ++i)
        mutate(data)->byteAt(i) = payloadAt(i);
    return data;
}

void CanFrame::Instance::setPayload(const String &data)
{
    setPayloadCount(data->count());
    for (int i = 0; i < data->count(); ++i)
        payloadAt(i) = data->byteAt(i);
}

void CanFrame::Instance::setValueUInt32(uint32_t value)
{
    setPayloadCount(4);
    payloadAt(0) = value & 0xFF;
    payloadAt(1) = (value >> 8) & 0xFF;
    payloadAt(2) = (value >> 16) & 0xFF;
    payloadAt(3) = (value >> 24) & 0xFF;
}

void CanFrame::Instance::updateInverse(CanFrame *invFrame) const
{
    (*invFrame)->setPayloadCount(payloadCount());
    for (int i = 0; i < payloadCount(); ++i)
        (*invFrame)->payloadAt(i) = ~payloadAt(i);
}

String CanFrame::Instance::copy(int i0, int i1) const
{
    if (i1 > payloadCount()) i1 = payloadCount();
    if (i0 < 0) i0 = 0;
    if (i1 < i0) i0 = i1;
    String data = String::allocate(i1 - i0);
    for (int i = i0; i < i1; ++i)
        mutate(data)->byteAt(i - i0) = payloadAt(i);
    return data;
}

String CanFrame::Instance::toString() const
{
    Format f;
    f << hex(canId(), 3) << " [" << payloadCount() << "]";
    if (payloadCount() > 0) f << " ";
    for (int i = 0; i < payloadCount(); ++i) {
        f << hex(payloadAt(i), 2);
        if (i != payloadCount() - 1) f << ".";
    }
    if (isRemoteTransmissionRequest()) f << " // RTR";
    return f;
}

String str(const CanFrame &frame)
{
    return frame->toString();
}

}} // namespace cc::can
