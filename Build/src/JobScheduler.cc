/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/JobScheduler>
#include <cc/build/JobServer>
#include <cc/build/Job>
#include <cc/System>
#include <cc/Channel>
#include <cc/AppendList>
#include <cc/Set>

namespace cc::build {

struct JobScheduler::State: public Object::State
{
    State(int concurrency):
        concurrency_{concurrency > 0 ? concurrency : System::concurrency()}
    {}

    void start()
    {
        if (started_) return;
        started_ = true;

        for (int i = 0; i < concurrency_; ++i) {
            serverPool_.pushBack(JobServer{requestChannel_, replyChannel_});
        }
    }

    void schedule(const Job &job)
    {
        if (job.countDown() == 0)
            requestChannel_.pushBack(job);
        else
            derivatives_.insert(job);

        ++totalCount_;
    }

    bool collect(Out<Job> finishedJob)
    {
        if (totalCount_ == 0) {
            finishedJob = Job{};
            return false;
        }

        start();

        if (finishCount_ == totalCount_) {
            finishedJob = Job{};
            return false;
        }

        Job job;
        if (!replyChannel_.popFront(&job)) return false;

        finishedJob = job;

        if (job.status() == 0) {
            for (Job derivative; job.getNextDerivative(&derivative);) {
                if (derivative.countDown() == 0) {
                    requestChannel_.pushBack(derivative);
                    derivatives_.remove(derivative);
                }
            }
        }
        else {
            status_ = job.status();
            serverPool_.deplete();
        }

        ++finishCount_;

        return true;
    }

    int concurrency_ { -1 };

    Channel<Job> requestChannel_;
    Channel<Job> replyChannel_;

    AppendList<JobServer> serverPool_;
    Set<Job> derivatives_;

    bool started_ { false };
    int status_ { 0 };
    int totalCount_ { 0 };
    int finishCount_ { 0 };
};

JobScheduler::JobScheduler(int concurrency):
    Object{new State{concurrency}}
{}

int JobScheduler::concurrency() const
{
    return me().concurrency_;
}

void JobScheduler::start()
{
    me().start();
}

void JobScheduler::schedule(const Job &job)
{
    me().schedule(job);
}

bool JobScheduler::collect(Out<Job> finishedJob)
{
    return me().collect(finishedJob);
}

int JobScheduler::status() const
{
    return me().status_;
}

int JobScheduler::totalCount() const
{
    return me().totalCount_;
}

int JobScheduler::finishCount() const
{
    return me().finishCount_;
}

const JobScheduler::State &JobScheduler::me() const
{
    return Object::me.as<State>();
}

JobScheduler::State &JobScheduler::me()
{
    return Object::me.as<State>();
}

} // namespace cc::build
