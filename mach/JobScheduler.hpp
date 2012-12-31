#ifndef MACH_JOBSCHEDULER_HPP
#define MACH_JOBSCHEDULER_HPP

#include <ftl/Thread.hpp>
#include <ftl/List.hpp>
#include "Job.hpp"

namespace mach
{

using namespace ftl;

class JobScheduler: public Instance
{
public:
	static Ref<JobScheduler, Owner> start(Ref<JobList> jobList, int concurrency = -1);

	inline int concurrency() const { return concurrency_; }

	bool collect(Ref<Job, Owner> *completedJob);

	inline int status() const { return status_; }

private:
	JobScheduler(Ref<JobList> jobList, int concurrency);

	int concurrency_;

	Ref<JobChannel, Owner> requestChannel_;
	Ref<JobChannel, Owner> replyChannel_;

	typedef List< Ref<JobServer, Owner> > JobServerList;
	Ref<JobServerList, Owner> serverPool_;

	int status_;
	int totalCount_;
	int finishCount_;
};

} // namespace mach

#endif // MACH_JOBSCHEDULER_HPP
