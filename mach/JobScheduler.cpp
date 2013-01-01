#include <ftl/System.hpp>
#include "JobServer.hpp"
#include "JobScheduler.hpp"

namespace mach
{

Ref<JobScheduler, Owner> JobScheduler::start(int concurrency)
{
	return new JobScheduler(concurrency);
}

JobScheduler::JobScheduler(int concurrency)
	: concurrency_((concurrency > 0) ? concurrency : (System::concurrency() + 1)),
	  requestChannel_(JobChannel::create()),
	  replyChannel_(JobChannel::create()),
	  serverPool_(ServerPool::create()),
	  status_(0),
	  totalCount_(0),
	  finishCount_(0)
{
	for (int i = 0; i < concurrency_; ++i)
		serverPool_->pushBack(JobServer::start(requestChannel_, replyChannel_));
}

void JobScheduler::schedule(Ref<Job> job)
{
	requestChannel_->pushBack(job);
	++totalCount_;
}

bool JobScheduler::collect(Ref<Job, Owner> *completedJob)
{
	if ((finishCount_ == totalCount_) || !serverPool_) {
		*completedJob = 0;
		return false;
	}

	Ref<Job, Owner> job = replyChannel_->popFront();
	*completedJob = job;
	if (job->status() != 0) {
		status_ = job->status();
		serverPool_ = 0;
	}
	++finishCount_;

	return true;
}

} // namespace mach
