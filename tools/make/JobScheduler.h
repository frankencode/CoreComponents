/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FMAKE_JOBSCHEDULER_H
#define FMAKE_JOBSCHEDULER_H

#include <fkit/Thread.h>
#include "Job.h"

namespace fmake
{

using namespace fkit;

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

} // namespace fmake

#endif // FMAKE_JOBSCHEDULER_H
