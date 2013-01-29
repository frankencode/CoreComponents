#ifndef MACH_JOB_HPP
#define MACH_JOB_HPP

#include <ftl/string.hpp>
#include <ftl/Channel.hpp>

namespace mach
{

using namespace ftl;

class JobServer;

class Job: public Instance
{
public:
	inline static hook<Job> create(string command) {
		return new Job(command);
	}

	inline string command() const { return command_; }

	inline int status() const { return status_; }
	inline string outputText() const { return outputText_; }

private:
	friend class JobServer;

	Job(string command)
		: command_(command),
		  status_(-1)
	{}

	string command_;

	int status_;
	string outputText_;
};

typedef Channel< hook<Job> > JobChannel;

} // namespace mach

#endif // MACH_JOB_HPP
