#include <ftl/ProcessFactory.hpp>
#include "JobServer.hpp"

namespace mach
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
	Ref<ProcessFactory, Owner> factory = ProcessFactory::create();
	factory->setIoPolicy(Process::CloseInput|Process::ForwardOutput|Process::ErrorToOutput);

	while (true) {
		Ref<Job, Owner> job = requestChannel_->popFront();
		if (!job) break;
		factory->setCommand(job->command_);
		Ref<Process, Owner> process = factory->produce();
		job->outputText_ = process->rawOutput()->readAll();
		job->status_ = process->wait();
		replyChannel_->pushBack(job);
	}
}

} // namespace mach
