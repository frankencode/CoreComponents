/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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
	String loginName() const;
	String commandName() const;

private:
	pid_t processId_;
	pid_t parentProcessId_;
	gid_t processGroupId_;
	gid_t foregroundProcessGroupId_;
	String loginName_;
	String commandName_;
};

} // namespace flux

#endif // FLUX_PROCESSSTATUS_H
