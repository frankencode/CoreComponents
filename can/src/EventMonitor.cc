/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/NextEvent>
#include <cc/can/EventMonitor>
#include <cc/System>
#include <cc/Set>
#include <cc/Heap>
#include <cc/Mutex>
#include <cc/Semaphore>
#include <cc/Worker>

namespace cc {
namespace can {

Ref<EventMonitor> EventMonitor::create(CanMedia *media)
{
    Ref<EventMonitor> monitor = new EventMonitor{media};
    monitor->init();
    return monitor;
}

EventMonitor::EventMonitor(CanMedia *media):
    media_{media},
    events_{Events::create()},
    mutex_{Mutex::create()},
    start_{Semaphore::create()},
    shutdown_{Semaphore::create()}
{}

EventMonitor::~EventMonitor()
{
    shutdown();
}

void EventMonitor::init()
{
    Ref<EventMonitor> self{this};
    worker_ = Worker::start([=]{ self->run(); });
}

void EventMonitor::registerEvent(Event *event)
{
    Guard<Mutex> guard{mutex_};
    events_->establish(event->canId(), event);
}

void EventMonitor::start()
{
    Guard<Mutex> guard{mutex_};
    if (events_->count() == 0) return;
    start_->release();
}

void EventMonitor::shutdown()
{
    shutdown_->release();
}

void EventMonitor::run()
{
    start_->acquire();

    Guard<Mutex> guard{mutex_};

    double now = System::now();
    auto timeLine = MinHeap<NextEvent>::create(events_->count());
    {
        for (int i = 0; i < events_->count(); ++i) {
            Event *event = events_->valueAt(i);
            if (event->autoStart()) {
                timeLine->push(
                    NextEvent{
                        now + event->initialInterval(),
                        event
                    }
                );
            }
        }
    }

    auto frame = CanFrame::create();
    now = System::now();

    while (true) {
        NextEvent next = timeLine->pop();

        while (true) {
            if (shutdown_->tryAcquire()) return;

            if (next->timeout() <= now) {
                if (!next->event()->busy_)
                    next->event()->onIdle(media_);
            }
            else {
                if (media_->waitFrame((next->timeout() - now) * 1000))
                {
                    if (!media_->readFrame(frame)) return;
                    Ref<Event> event;
                    if (!events_->lookup(frame->canId(), &event)) {
                        now = System::now();
                        continue;
                    }
                    event->feed(media_, frame->payload());
                    if (next->event()->canId() != event->canId()) {
                        now = System::now();
                        event->busy_ = true;
                        continue;
                    }
                }
                else {
                    if (!next->event()->busy_)
                        next->event()->onIdle(media_);
                }
            }

            break;
        }

        now = System::now();
        next->event()->timeAdvance(&next, now);
        next->event()->busy_ = false;
        timeLine->push(next);
    }
}

}} // namespace cc::can
