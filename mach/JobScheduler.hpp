#ifndef FTL_JOBSCHEDULER_HPP
#define FTL_JOBSCHEDULER_HPP

#include <ftl/Thread.hpp>
#include "Job.hpp"

namespace ftl {
class Semaphore;
class Mutex;
class ProcessFactory;
} // namespace ftl

namespace mach
{

using namespace ftl;

class JobScheduler: public Thread
{
public:
	static Ref<JobScheduler, Owner> start(Ref<JobList> jobList, int concurrency = -1);

	bool collect(Ref<Job> *completedJob);

	inline int status() const { return status_; }

private:
	JobScheduler(Ref<JobList> jobList, int concurrency);
	~JobScheduler();

	virtual void run();

	Ref<JobList, Owner> jobList_;
	Ref<Semaphore, Owner> slots_, running_;
	Ref<Mutex, Owner> starting_;
	Ref<ProcessFactory, Owner> factory_;
	Ref<RunMap, Owner> runMap_;
	int status_;
	int totalCount_;
	int finishCount_;
};

} // namespace mach

#endif // FTL_JOBSCHEDULER_HPP
