/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifdef __linux
#include "File.h"
#include "FileStatus.h"
#include "User.h"
#include "Format.h"
#include "LineSource.h"
#else
#include <sys/param.h>
#include <sys/sysctl.h> // sysctl
#include <sys/stat.h> // devname_r
#include "strings.h" // memclr
#include "User.h"
#endif
#include "exceptions.h"
#include "ProcessStatus.h"

namespace flux
{

ProcessStatus::ProcessStatus(pid_t processId)
{
#ifdef __linux
	String path = Format("/proc/%%/stat") << processId;
	Ref<LineSource> source = LineSource::open(File::open(path));
	String line = source->readLine();
	{
		// extract command name first, because it may contain whitespace
		int i0 = line->find('(') + 1, i1 = line->find(')');
		commandName_ = line->copy(i0, i1);
		for (int i = i0; i < i1; ++i)
			line->at(i) = 'x';
	}
	Ref<StringList> parts = line->split(" ");
	processId_ = parts->at(0)->toInt();
	parentProcessId_ = parts->at(3)->toInt();
	processGroupId_ = parts->at(4)->toInt();
	foregroundProcessGroupId_ = parts->at(7)->toInt();
	loginName_ = User::lookup(File::status(path)->ownerId())->loginName();
#else // def __linux
#ifdef __OpenBSD__
	size_t sz = sizeof(kinfo_proc);
	struct kinfo_proc *proc = (kinfo_proc *)flux::malloc(sz);
	int mib[6];
	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_PID;
	mib[3] = processId;
	mib[4] = sizeof(kinfo_proc);
	mib[5] = 1;
	if (::sysctl(mib, 6, proc, &sz, NULL, 0) == -1)
		FLUX_SYSTEM_DEBUG_ERROR(errno);
	processId_ = proc->p_pid;
	parentProcessId_ = proc->p_ppid;
	processGroupId_ = proc->p__pgid;
	foregroundProcessGroupId_ = proc->p_tpgid;
	loginName_ = User::lookup(proc->p_ruid)->loginName();
	commandName_ = proc->p_comm;
	flux::free(proc);
#else // def __OPENBSD__
	struct kinfo_proc *proc;
	int mib[4];
	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_PID;
	mib[3] = processId;
	size_t sz = 0;
	if (::sysctl(mib, 4, NULL, &sz, NULL, 0) == -1)
		FLUX_SYSTEM_DEBUG_ERROR(errno);
	proc = (kinfo_proc *)flux::malloc(sz);
	memclr(proc, sz);
	if (::sysctl(mib, 4, proc, &sz, NULL, 0) == -1)
		FLUX_SYSTEM_DEBUG_ERROR(errno);
	processId_ = proc->kp_proc.p_pid;
	parentProcessId_ = proc->kp_eproc.e_ppid;
	processGroupId_ = proc->kp_eproc.e_pgid;
	foregroundProcessGroupId_ = proc->kp_eproc.e_tpgid;
	loginName_ = User::lookup(proc->kp_eproc.e_pcred.p_ruid)->loginName();
	commandName_ = proc->kp_proc.p_comm;
	flux::free(proc);
#endif // def __OPENBSD__
#endif // def __linux
}

pid_t ProcessStatus::processId() const { return processId_; }
pid_t ProcessStatus::parentProcessId() const { return parentProcessId_; }
gid_t ProcessStatus::processGroupId() const { return processGroupId_; }
gid_t ProcessStatus::foregroundProcessGroupId() const { return foregroundProcessGroupId_; }
String ProcessStatus::loginName() const { return loginName_; }
String ProcessStatus::commandName() const { return commandName_; }

} // namespace flux
