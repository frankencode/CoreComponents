/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "JobServer.h"

namespace ccbuild {

JobServer::JobServer(const JobChannel &requestChannel, const JobChannel &replyChannel):
    requestChannel_{requestChannel},
    replyChannel_{replyChannel}
{
    Thread::start();
}

JobServer::~JobServer()
{
    requestChannel_->pushFront(nullptr);
    Thread::wait();
}

void JobServer::run()
{
    while (true) {
        Ref<Job> job = requestChannel_->popFront();
        if (!job) break;
        job->run(); // FIXME: shouldn't we stop scheduling if run() returns false?
        replyChannel_->pushBack(job);
    }
}

} // namespace ccbuild
