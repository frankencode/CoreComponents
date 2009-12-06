#ifndef __linux
#include <sys/param.h>
#include <sys/sysctl.h> // sysctl
#include <sys/stat.h> // devname_r
#include <string.h> // memset
#include "User.hpp"
#else
#include "File.hpp"
#include "FileStatus.hpp"
#include "User.hpp"
#include "Format.hpp"
#include "LineSource.hpp"
#endif

#include "ProcessStatus.hpp"

namespace pona
{

ProcessStatus::ProcessStatus(pid_t processId)
{
#ifndef __linux
	struct kinfo_proc* proc;
	int mib[4];
	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_PID;
	mib[3] = processId;
	size_t sz = 0;
	if (::sysctl(mib, 4, NULL, &sz, NULL, 0) == -1)
		PONA_SYSTEM_EXCEPTION;
	proc = (kinfo_proc*)pona::malloc(sz);
	::memset(proc, 0, sz);
	if (::sysctl(mib, 4, proc, &sz, NULL, 0) == -1)
		PONA_SYSTEM_EXCEPTION;
	processId_ = proc->kp_proc.p_pid;
	parentProcessId_ = proc->kp_eproc.e_ppid;
	processGroupId_ = proc->kp_eproc.e_pgid;
	foregroundProcessGroupId_ = proc->kp_eproc.e_tpgid;
	/*const int ttyNameSize = 256;
	char ttyName[ttyNameSize];
	terminalName_ = devname_r(proc->kp_eproc.e_tdev, S_IFCHR, ttyName, ttyNameSize);*/
	loginName_ = User(proc->kp_eproc.e_pcred.p_ruid).loginName();
	commandName_ = proc->kp_proc.p_comm;
	processStatus_ = proc->kp_proc.p_stat;
	if (processStatus_ == SIDL) processStatus_ = 'W';
	else if (processStatus_ == SRUN) processStatus_ = 'R';
#ifdef __MACH__
	else if (processStatus_ == SSLEEP) processStatus_ = (proc->kp_proc.sigwait) ? 'S' : 'D';
#else
	else if (processStatus_ == SSLEEP) processStatus_ = 'S';
#endif
	else if (processStatus_ == SSTOP) processStatus_ = 'T';
	else if (processStatus_ == SZOMB) processStatus_ = 'Z';
	pona::free(proc);
#else
	String path = Format("/proc/%%/stat") << processId;
	Ref<File, Owner> file = new File(path);
	file->open(File::Read);
	Ref<LineSource, Owner> source = new LineSource(file);
	String line = source->readLine();
	{
		// extract command name first, because it may contain whitespace
		int i0 = line->find('(') + 1, i1 = line->find(')');
		commandName_ = line->copy(i0, i1 - i0);
		line->fill(i0, i1 - i0, 'x');
	}
	Ref<StringList, Owner> parts = line / ' ';
	processId_ = toInt(parts->get(0));
	parentProcessId_ = toInt(parts->get(3));
	processGroupId_ = toInt(parts->get(4));
	foregroundProcessGroupId_ = toInt(parts->get(7));
	/*{
		int code = toInt(parts->get(6));
		int major = (code >> 8) & 0xFF;
		int minor = (code & 0xFF) | ((code >> 20) << 8);
		 // interpretation according to lanana.org
		if (major == 4)
			terminalName_ = Format("tty%%") << minor;
		else if ((136 <= major) && (major <= 143))
			terminalName_ = Format("pts/%%") << minor;
		else if (major == 3)
			terminalName_ = Format("ttyp%%") << minor;
	}*/
	loginName_ = User(FileStatus(path).ownerId()).loginName();
	processStatus_ = parts->get(2)->get(0);
	file->close();
#endif
}

pid_t ProcessStatus::processId() const { return processId_; }
pid_t ProcessStatus::parentProcessId() const { return parentProcessId_; }
gid_t ProcessStatus::processGroupId() const { return processGroupId_; }
gid_t ProcessStatus::foregroundProcessGroupId() const { return foregroundProcessGroupId_; }
// String ProcessStatus::terminalName() const { return terminalName_; }
String ProcessStatus::loginName() const { return loginName_; }
String ProcessStatus::commandName() const { return commandName_; }
char ProcessStatus::processStatus() const { return processStatus_; }

} // namespace pona
