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
    static Ref<JobServer> start(const JobChannel &requestChannel, const JobChannel &replyChannel) {
        return new JobServer{requestChannel, replyChannel};
    }

private:
    JobServer(const JobChannel &requestChannel, const JobChannel &replyChannel);
    ~JobServer();
    virtual void run();

    JobChannel requestChannel_;
    JobChannel replyChannel_;
};

} // namespace ccbuild
