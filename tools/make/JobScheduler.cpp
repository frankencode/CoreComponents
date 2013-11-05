/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/System.h>
#include "JobServer.h"
#include "JobScheduler.h"

namespace fmake
{

Ref<JobScheduler> JobScheduler::create(int concurrency)
{
	return new JobScheduler(concurrency);
}

JobScheduler::JobScheduler(int concurrency)
	: concurrency_((concurrency > 0) ? concurrency : (System::concurrency() + 1)),
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
	requestChannel_->pushBack(job);
	++totalCount_;
}

bool JobScheduler::collect(Ref<Job> *completedJob)
{
	start();
	if ((finishCount_ == totalCount_) || !serverPool_) {
		*completedJob = 0;
		return false;
	}

	Ref<Job> job = replyChannel_->popFront();
	*completedJob = job;
	if (job->status() != 0) {
		status_ = job->status();
		serverPool_ = 0;
	}
	++finishCount_;

	return true;
}

} // namespace fmake
