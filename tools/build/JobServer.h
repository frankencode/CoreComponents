/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Thread>
#include <cc/Channel>
#include "Job.h"

namespace ccbuild {

class JobServer: public Thread
{
public:
    inline static Ref<JobServer> start(JobChannel *requestChannel, JobChannel *replyChannel) {
        return new JobServer(requestChannel, replyChannel);
    }

private:
    JobServer(JobChannel *requestChannel, JobChannel *replyChannel);
    ~JobServer();
    virtual void run();

    Ref<JobChannel> requestChannel_;
    Ref<JobChannel> replyChannel_;
};

} // namespace ccbuild

