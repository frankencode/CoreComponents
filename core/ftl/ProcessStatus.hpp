/*
 * ProcessStatus.hpp -- status information about process
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_PROCESSSTATUS_HPP
#define FTL_PROCESSSTATUS_HPP

#include <sys/types.h>
#include "atoms"
#include "String.hpp"

namespace ftl
{

class ProcessStatus: public Instance
{
public:
	ProcessStatus(pid_t processId);
	
	pid_t processId() const;
	pid_t parentProcessId() const;
	gid_t processGroupId() const;
	gid_t foregroundProcessGroupId() const;
	// String terminalName() const;
	String loginName() const;
	String commandName() const;
	char processStatus() const;
	
private:
	pid_t processId_;
	pid_t parentProcessId_;
	gid_t processGroupId_;
	gid_t foregroundProcessGroupId_;
	// String terminalName_;
	String loginName_;
	String commandName_;
	char processStatus_;
};

} // namespace ftl

#endif // FTL_PROCESSSTATUS_HPP
