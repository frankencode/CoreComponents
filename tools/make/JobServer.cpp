/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/ProcessFactory.h>
#include "JobServer.h"

namespace fmake
{

JobServer::JobServer(JobChannel *requestChannel, JobChannel *replyChannel)
	: requestChannel_(requestChannel),
	  replyChannel_(replyChannel)
{
	Thread::start();
}

JobServer::~JobServer()
{
	requestChannel_->pushFront(0);
	wait();
}

void JobServer::run()
{
	Ref<ProcessFactory> factory = ProcessFactory::create();
	factory->setIoPolicy(Process::CloseInput|Process::ForwardOutput|Process::ErrorToOutput);

	while (true) {
		Ref<Job> job = requestChannel_->popFront();
		if (!job) break;
		factory->setCommand(job->command_);
		Ref<Process> process = factory->produce();
		job->outputText_ = process->out()->readAll();
		job->status_ = process->wait();
		replyChannel_->pushBack(job);
	}
}

} // namespace fmake
