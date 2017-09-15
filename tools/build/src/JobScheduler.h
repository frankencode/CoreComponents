/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Thread>
#include <cc/Set>
#include "Job.h"

namespace ccbuild {

using namespace cc;

/** \brief Background job scheduler
  * \note The JobScheduler itself is note meant to be shared between threads
  */
class JobScheduler: public Object
{
public:
    static Ref<JobScheduler> create(int concurrency = -1);

    inline int concurrency() const { return concurrency_; }

    /// Start executing jobs in the background
    void start();

    /// Schedule \a job for execution in the background
    void schedule(Job *job);

    /** \brief Collect the next finished job (\a finishedJob)
      * \return true if \a finishedJob is returned otherwise return false (end of operation)
      */
    bool collect(Ref<Job> *finishedJob);

    /// Exit status of the first unsuccessfully finished job, or 0 if all jobs completed successully
    inline int status() const { return status_; }

    inline int totalCount() const { return totalCount_; }
    inline int finishCount() const { return finishCount_; }

private:
    JobScheduler(int concurrency);

    int concurrency_;

    Ref<JobChannel> requestChannel_;
    Ref<JobChannel> replyChannel_;

    typedef Queue< Ref<JobServer> > ServerPool;
    Ref<ServerPool> serverPool_;

    typedef Set< Ref<Job> > Derivatives;
    Ref<Derivatives> derivatives_;

    bool started_;
    int status_;
    int totalCount_;
    int finishCount_;
};

} // namespace ccbuild
