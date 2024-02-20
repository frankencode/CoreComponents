/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/WaitableJob>
#include <cc/build/JobState>
#include <cc/Semaphore>

namespace cc::build {

struct WaitableJob::State final: public Job::State
{
    explicit State(const String &command):
        Job::State{command}
    {}

    bool run() override
    {
        bool success = Job::State::run();

        ready_.release();

        return success;
    }

    void wait()
    {
        ready_.acquire();
        ready_.release();
    }

    Semaphore<int> ready_;
};


WaitableJob::WaitableJob(const String &command):
    Job{new State{command}}
{}

void WaitableJob::wait()
{
    me().wait();
}

void WaitableJob::notify()
{
    me().ready_.release();
}

WaitableJob::State &WaitableJob::me()
{
    return Object::me.as<State>();
}

} // namespace cc::build
