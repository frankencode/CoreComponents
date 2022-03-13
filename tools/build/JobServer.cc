/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/build/JobServer>

namespace cc::build {

struct JobServer::State: public Object::State
{
    State(const Channel<Job> &requestChannel, const Channel<Job> &replyChannel):
        requestChannel_{requestChannel},
        replyChannel_{replyChannel},
        thread_{[&]{ run(); }}
    {
        thread_.start();
    }

    ~State()
    {
        requestChannel_.shutdown();
        thread_.wait();
    }

    void run()
    {
        for (Job job; requestChannel_.popFront(&job);) {
            job.run();
            replyChannel_.pushBack(job);
        }
    }

    Channel<Job> requestChannel_;
    Channel<Job> replyChannel_;
    Thread thread_;
};

JobServer::JobServer(const Channel<Job> &requestChannel, const Channel<Job> &replyChannel):
    Object{new State{requestChannel, replyChannel}}
{}

} // namespace cc::build
