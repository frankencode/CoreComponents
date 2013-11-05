/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_PROCESSSTATUS_H
#define FLUX_PROCESSSTATUS_H

#include <sys/types.h>
#include "String.h"

namespace flux
{

class ProcessStatus: public Object
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

} // namespace flux

#endif // FLUX_PROCESSSTATUS_H
