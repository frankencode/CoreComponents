#include <ftl/Guard.hpp>
#include <ftl/System.hpp>
#include <ftl/ProcessFactory.hpp>
#include <ftl/File.hpp>
#include "JobServer.hpp"
#include "JobScheduler.hpp"

namespace mach
{

Ref<JobScheduler, Owner> JobScheduler::start(Ref<JobList> jobList, int concurrency)
{
	return new JobScheduler(jobList, concurrency);
}

JobScheduler::JobScheduler(Ref<JobList> jobList, int concurrency)
	: concurrency_((concurrency > 0) ? concurrency : System::concurrency()),
	  requestChannel_(JobChannel::create()),
	  replyChannel_(JobChannel::create()),
	  serverPool_(JobServerList::create(concurrency_)),
	  status_(0),
	  totalCount_(jobList->length()),
	  finishCount_(0)
{
	for (int i = 0; i < jobList->length(); ++i)
		requestChannel_->push(jobList->at(i));
	for (int i = 0; i < serverPool_->length(); ++i)
		serverPool_->set(i, JobServer::start(requestChannel_, replyChannel_));
}

bool JobScheduler::collect(Ref<Job, Owner> *completedJob)
{
	if ((finishCount_ == totalCount_) || !serverPool_) {
		*completedJob = 0;
		return false;
	}

	Ref<Job, Owner> job = replyChannel_->pop();
	*completedJob = job;
	if (job->status() != 0) {
		status_ = job->status();
		serverPool_ = 0;
	}
	++finishCount_;

	return job;
}

} // namespace mach
