/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/SafetyEmission>
#include <cc/can/CanMedia>

namespace cc {
namespace can {

Ref<SafetyEmission> SafetyEmission::create(PeriodicEmitter *emitter, int canId, int invCanId, double interval, const String &initialData)
{
    Ref<SafetyEmission> emission = new SafetyEmission{canId, invCanId, interval};
    emission->init(emitter, initialData);
    return emission;
}

SafetyEmission::SafetyEmission(int canId, int invCanId, double interval):
    Emission{canId, interval},
    invFrame_{CanFrame::create()}
{
    invFrame_->setCanId(invCanId);
}

void SafetyEmission::updateData(const String &data)
{
    Guard<Emission> guard(this);

    const int n = data->count();

    frame_->setPayloadCount(n);
    for (int i = 0; i < n; ++i)
        frame_->payloadAt(i) = data->byteAt(i);

    invFrame_->setPayloadCount(n);
    for (int i = 0; i < n; ++i)
        invFrame_->payloadAt(i) = ~data->byteAt(i);
}

void SafetyEmission::generate(CanMedia *media)
{
    Guard<Emission> guard(this);
    media->writeFrame(frame_);
    media->writeFrame(invFrame_);
}

}} // namespace cc::can
