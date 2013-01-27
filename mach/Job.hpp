#ifndef MACH_JOB_HPP
#define MACH_JOB_HPP

#include <ftl/String.hpp>
#include <ftl/Channel.hpp>

namespace mach
{

using namespace ftl;

class JobServer;

class Job: public Instance
{
public:
	inline static O<Job> create(String command) {
		return new Job(command);
	}

	inline String command() const { return command_; }

	inline int status() const { return status_; }
	inline String outputText() const { return outputText_; }

private:
	friend class JobServer;

	Job(String command)
		: command_(command),
		  status_(-1)
	{}

	String command_;

	int status_;
	String outputText_;
};

typedef Channel< O<Job> > JobChannel;

} // namespace mach

#endif // MACH_JOB_HPP
