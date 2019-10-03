/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/SafetyEvent>
#include <cc/can/EventMonitor>
#include <cc/can/NextEvent>

namespace cc {
namespace can {

Ref<SafetyEvent> SafetyEvent::create(EventMonitor *monitor, int canId, int invCanId, double idleInterval, double validationInterval)
{
    return make<SafetyEvent>(monitor, canId, invCanId, idleInterval, validationInterval);
}

SafetyEvent::SafetyEvent(int canId, double idleInterval):
    Event{canId, idleInterval}
{}

void SafetyEvent::onError(CanMedia *media, SafetyError error)
{}

void SafetyEvent::onIdle(CanMedia *media)
{
    if (isPrimaryEvent()) onError(media, SafetyError::RefreshTimeout);
    else primaryEvent_->onError(media, SafetyError::ValidationTimeout);
}

bool SafetyEvent::isPrimaryEvent() const
{
    return primaryEvent_ == nullptr;
}

void SafetyEvent::feed(CanMedia *media, const String &data)
{
    if (isPrimaryEvent()) {
        lastData_ = data;
        hasLastData_ = true;
    }
    else if (primaryEvent_->hasLastData_) {
        String primaryData = primaryEvent_->lastData_;
        if (primaryData->count() != data->count()) {
            primaryEvent_->onError(media, SafetyError::DataValidation);
            return;
        }
        for (int i = 0; i < primaryData->count(); ++i) {
            if (primaryData->byteAt(i) != uint8_t(~data->byteAt(i))) {
                primaryEvent_->onError(media, SafetyError::DataValidation);
                return;
            }
        }
        primaryEvent_->onReceived(media, primaryData);
        primaryEvent_->hasLastData_ = false;
    }
}

void SafetyEvent::timeAdvance(NextEvent *next, double now)
{
    if (isPrimaryEvent()) {
        lastOccurrence_ = now;
        *next = NextEvent{
            now + inverseEvent_->idleInterval(),
            inverseEvent_
        };
    }
    else {
        *next = NextEvent{
            primaryEvent_->lastOccurrence_ + primaryEvent_->idleInterval(),
            primaryEvent_
        };
    }
}

}} // namespace cc::can
