/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/System>
#include "JobServer.h"
#include "JobScheduler.h"

namespace ccbuild {

Ref<JobScheduler> JobScheduler::create(int concurrency)
{
    return new JobScheduler(concurrency);
}

JobScheduler::JobScheduler(int concurrency):
    concurrency_((concurrency > 0) ? concurrency : System::concurrency()),
    requestChannel_(JobChannel::create()),
    replyChannel_(JobChannel::create()),
    started_(false),
    status_(0),
    totalCount_(0),
    finishCount_(0)
{}

void JobScheduler::start()
{
    if (started_) return;
    started_ = true;
    serverPool_ = ServerPool::create();
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

void JobScheduler::schedule(Job *job, Job *derivative)
{
    if (!job->derivatives_) job->derivatives_ = Job::Derivatives::create();
    job->derivatives_->pushBack(derivative);
    ++derivative->countDown_;
}

bool JobScheduler::collect(Ref<Job> *finishedJob)
{
    start();
    if ((finishCount_ == totalCount_) || !serverPool_) {
        *finishedJob = 0;
        return false;
    }

    Ref<Job> job = replyChannel_->popFront();
    *finishedJob = job;
    if (job->status() == 0) {
        if (job->derivatives_) {
            for (Ref<Job> derivative; job->derivatives_->popFront(&derivative);) {
                --derivative->countDown_;
                if (derivative->countDown_ == 0) {
                    requestChannel_->pushBack(derivative);
                    derivatives_->remove(derivative);
                }
            }
        }
        job->onCompleted();
    }
    else {
        status_ = job->status();
        serverPool_ = 0;
    }
    ++finishCount_;

    return true;
}

} // namespace ccbuild
