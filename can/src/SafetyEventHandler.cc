/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/SafetyEventHandler>

namespace cc {
namespace can {

Ref<SafetyEventHandler> SafetyEventHandler::create(
    EventMonitor *monitor,
    int canId, int invCanId,
    double idleInterval, double validationInterval,
    OnReceived onReceived, OnError onError
)
{
    return make<SafetyEventHandler>(monitor, canId, invCanId, idleInterval, validationInterval, onReceived, onError);
}

SafetyEventHandler::SafetyEventHandler(
    int canId, double idleInterval,
    OnReceived onReceived, OnError onError
):
    SafetyEvent{canId, idleInterval},
    onReceived_{onReceived},
    onError_{onError}
{}

void SafetyEventHandler::onReceived(CanMedia *media, const String &data)
{
    onReceived_(data);
}

void SafetyEventHandler::onError(CanMedia *media, SafetyError error)
{
    onError_(error);
}

}} // namespace cc::can
