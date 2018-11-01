/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Thread>
#include <cc/Worker>

namespace cc {

class WorkerThread: public Thread {
private:
    friend class Worker;

    WorkerThread(const std::function<void()> &f):
        f_{f}
    {}

    void run() override
    {
        f_();
    }

    std::function<void()> f_;
};

Ref<Worker> Worker::start(const std::function<void()> &f)
{
    return new Worker{f};
}

Worker::Worker(const std::function<void()> &f):
    thread_{new WorkerThread{f}}
{
    thread_->start();
}

Worker::~Worker()
{
    thread_->wait();
}

} // namespace cc
