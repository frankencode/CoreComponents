#include <ftl/System.hpp>
#include "JobServer.hpp"
#include "JobScheduler.hpp"

namespace mach
{

O<JobScheduler> JobScheduler::create(int concurrency)
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

bool JobScheduler::collect(O<Job> *completedJob)
{
	start();
	if ((finishCount_ == totalCount_) || !serverPool_) {
		*completedJob = 0;
		return false;
	}

	O<Job> job = replyChannel_->popFront();
	*completedJob = job;
	if (job->status() != 0) {
		status_ = job->status();
		serverPool_ = 0;
	}
	++finishCount_;

	return true;
}

} // namespace mach
