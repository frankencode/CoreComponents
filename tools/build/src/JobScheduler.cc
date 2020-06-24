/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "JobScheduler.h"
#include "JobServer.h"
#include <cc/System>

namespace ccbuild {

Ref<JobScheduler> JobScheduler::create(int concurrency)
{
    return new JobScheduler{concurrency};
}

JobScheduler::JobScheduler(int concurrency):
    concurrency_{(concurrency > 0) ? concurrency : System::concurrency()},
    started_{false},
    status_{0},
    totalCount_{0},
    finishCount_{0}
{}

void JobScheduler::start()
{
    if (started_) return;
    started_ = true;
    for (int i = 0; i < concurrency_; ++i)
        serverPool_->pushBack(JobServer::start(requestChannel_, replyChannel_));
}

void JobScheduler::schedule(Job *job)
{
    if (job->countDown_ == 0)
        requestChannel_->pushBack(job);
    else
        derivatives_->insert(job);
    ++totalCount_;
}

bool JobScheduler::collect(Ref<Job> *finishedJob)
{
    if (totalCount_ == 0) {
        *finishedJob = nullptr;
        return false;
    }

    start();

    if ((finishCount_ == totalCount_) || !serverPool_) {
        *finishedJob = nullptr;
        return false;
    }

    Ref<Job> job = replyChannel_->popFront();
    *finishedJob = job;
    if (job->status() == 0) {
        if (job->derivatives_) {
            while (job->derivatives_->count() > 0) {
                Ref<Job> derivative = job->derivatives_->popFront();
                --derivative->countDown_;
                if (derivative->countDown_ == 0) {
                    requestChannel_->pushBack(derivative);
                    derivatives_->remove(derivative);
                }
            }
        }
    }
    else {
        status_ = job->status();
        serverPool_ = 0;
    }
    ++finishCount_;

    return true;
}

} // namespace ccbuild
