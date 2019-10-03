/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/Emission>
#include <cc/can/PeriodicEmitter>
#include <cc/can/CanMedia>
#include <cc/Mutex>

namespace cc {
namespace can {

Ref<Emission> Emission::create(PeriodicEmitter *emitter, int canId, double interval, const String &initialData)
{
    Ref<Emission> emission = new Emission{canId, interval};
    emission->init(emitter, initialData);
    return emission;
}

Emission::Emission(int canId, double interval):
    interval_{interval},
    frame_{CanFrame::create()},
    mutex_{Mutex::create()}
{
    frame_->setCanId(canId);
}

Emission::~Emission()
{}

void Emission::init(PeriodicEmitter *emitter, const String &initialData)
{
    updateData(initialData);
    emitter->registerEmission(this);
}

void Emission::updateData(const String &data)
{
    Guard<Emission> guard(this);
    const int n = data->count();
    frame_->setPayloadCount(n);
    for (int i = 0; i < n; ++i)
        frame_->payloadAt(i) = data->byteAt(i);
}

void Emission::generate(CanMedia *media)
{
    Guard<Emission> guard(this);
    media->writeFrame(frame_);
}

void Emission::acquire()
{
    mutex_->acquire();
}

void Emission::release()
{
    mutex_->release();
}

}} // namespace cc::can
