/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/EventHandler>
#include <cc/can/EventMonitor>

namespace cc {
namespace can {

Ref<EventHandler> EventHandler::create(
    EventMonitor *monitor, int canId, double idleInterval,
    const OnReceived &onReceived,
    const OnIdle &onIdle
)
{
    Ref<EventHandler> handler = new EventHandler{canId, idleInterval, onReceived, onIdle};
    handler->init(monitor);
    return handler;
}

EventHandler::EventHandler(
    int canId, double idleInterval,
    const OnReceived &onReceived,
    const OnIdle &onIdle
):
    Event{canId, idleInterval},
    onReceived_{onReceived},
    onIdle_{onIdle}
{}

void EventHandler::onReceived(CanMedia *media, const String &data)
{
    onReceived_(data);
}

void EventHandler::onIdle(CanMedia *media)
{
    onIdle_();
}

}} // namespace cc::can
