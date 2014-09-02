/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/ProcessFactory.h>
#include "JobServer.h"

namespace fluxmake
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

} // namespace fluxmake
