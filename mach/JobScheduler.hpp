#ifndef MACH_JOBSCHEDULER_HPP
#define MACH_JOBSCHEDULER_HPP

#include <ftl/Thread.hpp>
#include "Job.hpp"

namespace mach
{

using namespace ftl;

class JobScheduler: public Instance
{
public:
	static Ref<JobScheduler, Owner> create(int concurrency = -1);

	inline int concurrency() const { return concurrency_; }

	void start();
	void schedule(Job *job);
	bool collect(Ref<Job, Owner> *completedJob);

	inline int status() const { return status_; }
	inline int totalCount() const { return totalCount_; }
	inline int finishCount() const { return finishCount_; }

private:
	JobScheduler(int concurrency);

	int concurrency_;

	Ref<JobChannel, Owner> requestChannel_;
	Ref<JobChannel, Owner> replyChannel_;

	typedef Queue< Ref<JobServer, Owner> > ServerPool;
	Ref<ServerPool, Owner> serverPool_;

	bool started_;
	int status_;
	int totalCount_;
	int finishCount_;
};

} // namespace mach

#endif // MACH_JOBSCHEDULER_HPP
