/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/PeriodicEmitter>
#include <cc/can/NextEmission>
#include <cc/System>
#include <cc/Heap>

namespace cc {
namespace can {

Ref<PeriodicEmitter> PeriodicEmitter::create(const CanMedia &media)
{
    Ref<PeriodicEmitter> emitter = new PeriodicEmitter{media};
    emitter->init();
    return emitter;
}

PeriodicEmitter::PeriodicEmitter(const CanMedia &media):
    media_{media}
{}

PeriodicEmitter::~PeriodicEmitter()
{
    shutdown();
}

void PeriodicEmitter::init()
{
    Ref<PeriodicEmitter> self{this};
    worker_ = Worker{[=]{
        self->run();
    }};
}

void PeriodicEmitter::registerEmission(Emission *emission)
{
    Guard<Mutex> guard{mutex_};
    if (emissions_)
        emissions_->insert(emission);
}

void PeriodicEmitter::start()
{
    Guard<Mutex> guard{mutex_};
    if (!emissions_) return;
    if (emissions_->count() == 0) return;
    start_->release();
}

void PeriodicEmitter::shutdown()
{
    shutdown_->release();
}

void PeriodicEmitter::run()
{
    start_->acquire();

    auto timeLine = MinHeap<NextEmission>::create(emissions_->count());
    {
        Guard<Mutex> guard{mutex_};

        double now = System::now();
        for (int i = 0; i < emissions_->count(); ++i) {
            Emission *emission = emissions_->at(i);
            timeLine->push(
                NextEmission{
                    now + emission->interval(),
                    emission
                }
            );
            emission->generate(media_);
        }

        emissions_ = nullptr;
    }

    while (!shutdown_->tryAcquire()) {
        NextEmission next = timeLine->pop();
        double now = System::now();
        if (next->time() > now) Thread::sleep(next->time() - now);
        next->emission()->generate(media_);
        timeLine->push(
            NextEmission{
                next->time() + next->emission()->interval(),
                next->emission()
            }
        );
    }
}

}} // namespace cc::can
