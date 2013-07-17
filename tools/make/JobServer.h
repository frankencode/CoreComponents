/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FMAKE_JOBSERVER_H
#define FMAKE_JOBSERVER_H

#include <fkit/Thread.h>
#include <fkit/Channel.h>
#include "Job.h"

namespace fmake
{

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

} // namespace fmake

#endif // FMAKE_JOBSERVER_H
