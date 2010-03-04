#ifndef PONA_PROCESSSTATUS_HPP
#define PONA_PROCESSSTATUS_HPP

#include <sys/types.h>
#include "atoms"
#include "UString.hpp"

namespace pona
{

class ProcessStatus: public Instance
{
public:
	ProcessStatus(pid_t processId);
	
	pid_t processId() const;
	pid_t parentProcessId() const;
	gid_t processGroupId() const;
	gid_t foregroundProcessGroupId() const;
	// UString terminalName() const;
	UString loginName() const;
	UString commandName() const;
	char processStatus() const;
	
private:
	pid_t processId_;
	pid_t parentProcessId_;
	gid_t processGroupId_;
	gid_t foregroundProcessGroupId_;
	// UString terminalName_;
	UString loginName_;
	UString commandName_;
	char processStatus_;
};

} // namespace pona

#endif // PONA_PROCESSSTATUS_HPP
