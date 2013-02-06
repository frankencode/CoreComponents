#ifndef MACH_JOBSERVER_HPP
#define MACH_JOBSERVER_HPP

#include <ftl/Thread.hpp>
#include <ftl/Channel.hpp>
#include "Job.hpp"

namespace mach
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

} // namespace mach

#endif // MACH_JOBSERVER_HPP
