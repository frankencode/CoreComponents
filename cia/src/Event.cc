/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/Event>
#include <cc/can/NextEvent>
#include <cc/can/EventMonitor>

namespace cc {
namespace can {

Ref<Event> Event::create(EventMonitor *monitor, int canId, double idleInterval)
{
    CC_ASSERT(idleInterval > 0);
    Ref<Event> event = new Event(canId, idleInterval);
    event->init(monitor);
    return event;
}

Event::Event(int canId, double idleInterval):
    canId_{canId},
    idleInterval_{idleInterval},
    busy_{false}
{}

void Event::init(EventMonitor *monitor)
{
    monitor->registerEvent(this);
}

void Event::onReceived(CanMedia *media, const String &data)
{}

void Event::onIdle(CanMedia *media)
{}

void Event::feed(CanMedia *media, const String &data)
{
    onReceived(media, data);
}

void Event::timeAdvance(NextEvent *next, double now)
{
    (*next)->setTimeout(now + idleInterval_);
}

}} // namespace cc::can
