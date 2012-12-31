#ifndef FTL_JOB_HPP
#define FTL_JOB_HPP

#include <ftl/Process.hpp>

namespace mach
{

using namespace ftl;

class JobScheduler;

class Job: public Instance
{
public:
	inline static Ref<Job, Owner> create(String command) {
		return new Job(command);
	}

	inline String command() const { return command_; }

	inline int status() const { return status_; }
	inline String outputText() const { return outputText_; }

private:
	friend class JobScheduler;

	Job(String command)
		: command_(command),
		  status_(-1)
	{}

	String command_;
	Ref<Process, Owner> process_;
	int status_;
	String outputText_;
};

typedef List< Ref<Job, Owner> > JobList;
typedef Map<pid_t, Ref<Job, Owner> > RunMap;

} // namespace mach

#endif // FTL_JOB_HPP
