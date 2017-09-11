/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Thread>
#include "Job.h"

namespace ccbuild {

using namespace cc;

class JobScheduler: public Object
{
public:
    static Ref<JobScheduler> create(int concurrency = -1);

    inline int concurrency() const { return concurrency_; }

    void start();
    void schedule(Job *job);
    bool collect(Ref<Job> *completedJob);

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

    bool started_;
    int status_;
    int totalCount_;
    int finishCount_;
};

} // namespace ccbuild
