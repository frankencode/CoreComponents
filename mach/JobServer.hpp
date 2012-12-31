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
	inline static Ref<JobServer, Owner> start(Ref<JobChannel> requestChannel, Ref<JobChannel> replyChannel) {
		return new JobServer(requestChannel, replyChannel);
	}

private:
	JobServer(Ref<JobChannel> requestChannel, Ref<JobChannel> replyChannel);
	~JobServer();
	virtual void run();

	Ref<JobChannel, Owner> requestChannel_;
	Ref<JobChannel, Owner> replyChannel_;
};

} // namespace mach

#endif // MACH_JOBSERVER_HPP
