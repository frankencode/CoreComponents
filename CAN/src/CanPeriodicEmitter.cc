/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/CanPeriodicEmitter>
#include <cc/System>
#include <cc/Thread>
#include <cc/Semaphore>
#include <cc/Mutex>
#include <cc/Heap>
#include <cc/Set>

namespace cc {

struct CanPeriodicEmitter::State final: public Object::State
{
    struct NextEmission
    {
        NextEmission()
        {}

        NextEmission(double time, const CanPeriodicEmission &emission):
            time_{time},
            emission_{emission}
        {}

        double time() const { return time_; }
        CanPeriodicEmission &emission() { return emission_; }

        std::strong_ordering operator<=>(const NextEmission &other) const
        {
            std::partial_ordering o = time_ <=> other.time_;
            if (o == std::partial_ordering::less) return std::strong_ordering::less;
            if (o == std::partial_ordering::equivalent) return emission_.canId() <=> other.emission_.canId();
            return std::strong_ordering::greater;
        }

    private:
        double time_ { 0 };
        CanPeriodicEmission emission_;
    };

    explicit State(const CanMedia &media):
        media_{media}
    {
        thread_ = Thread{[this]{ run(); }};
        thread_.start();
    }

    ~State()
    {
        shutdown();
    }

    void addEmission(const CanPeriodicEmission &emission)
    {
        Guard<Mutex> guard { mutex_ };
        emissions_.insert(emission);
    }

    void start()
    {
        Guard<Mutex> guard { mutex_ };
        if (emissions_.count() == 0) return;
        start_.release();
    }

    void shutdown()
    {
        if (thread_) {
            shutdown_.release();
            thread_.wait();
            thread_ = Thread{};
        }
    }

    void run()
    {
        start_.acquire();

        Heap<NextEmission> timeLine { Dim<>{ emissions_.count() } };

        {
            Guard<Mutex> guard{mutex_};

            double now = System::now();
            for (CanPeriodicEmission emission: emissions_) {
                timeLine.push({now + emission.interval(), emission});
                emission.generate(media_);
            }

            emissions_.deplete();
        }

        while (!shutdown_.tryAcquire()) {
            NextEmission next = timeLine.pop();
            double now = System::now();
            if (next.time() > now) Thread::sleep(next.time() - now);
            next.emission().generate(media_);
            timeLine.push({next.time() + next.emission().interval(), next.emission()});
        }
    }

    CanMedia media_;

    Set<CanPeriodicEmission> emissions_;
    Mutex mutex_;

    Thread thread_;
    Semaphore<int> start_;
    Semaphore<int> shutdown_;
};

CanPeriodicEmitter::CanPeriodicEmitter(const CanMedia &media):
    Object{new State{media}}
{}

CanPeriodicEmitter &CanPeriodicEmitter::addEmission(const CanPeriodicEmission &emission)
{
    me().addEmission(emission);
    return *this;
}

void CanPeriodicEmitter::start()
{
    me().start();
}

void CanPeriodicEmitter::shutdown()
{
    me().shutdown();
}

CanPeriodicEmitter::State &CanPeriodicEmitter::me()
{
    return Object::me.as<State>();
}

} // namespace cc
