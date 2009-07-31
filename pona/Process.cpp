/*
 * Process.cpp -- child process
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <sys/wait.h> // waitpid
#include <sys/stat.h> // umask
#include <errno.h> // errno
#include <stdlib.h> // malloc, free, exit
#include <unistd.h> // chdir, readlink
#include <time.h> // nanosleep
#include <errno.h> // errno
#ifdef __linux
#include "stdio" // DEBUG
#include "File.hpp" // to read /proc
#include "Format.hpp"
#endif
#ifdef __MACH__
#include <mach-o/dyld.h> // _NSGetExecutablePath
#include <string.h> // memset
#endif
#include "Process.hpp"

extern "C" char** environ;

namespace pona
{

Process::Process(
	int type,
	int ioPolicy,
	Ref<SystemStream> rawInput,
	Ref<SystemStream> rawOutput,
	Ref<SystemStream> rawError,
	pid_t processId
)
	: type_(type),
	  ioPolicy_(ioPolicy),
	  rawInput_(rawInput),
	  rawOutput_(rawOutput),
	  rawError_(rawError),
	  processId_(processId)
{
	if (rawInput) input_ = new LineSink(rawInput);
	if (rawOutput) output_ = new LineSource(rawOutput);
	if (rawError) error_ = new LineSource(rawError);
}

int Process::type() const { return type_; }
int Process::ioPolicy() const { return ioPolicy_; }

Ref<SystemStream> Process::rawInput() const { return rawInput_; }
Ref<SystemStream> Process::rawOutput() const { return rawOutput_; }
Ref<SystemStream> Process::rawError() const { return rawError_; }

Ref<LineSink> Process::input() const { return input_; }
Ref<LineSource> Process::output() const { return output_; }
Ref<LineSource> Process::error() const { return error_; }

pid_t Process::processId() const { return processId_; }

void Process::kill(int signal)
{
	if (type() == GroupMember)
		Process::signal(processId_, signal);
	else
		Process::broadcast(processId_, signal);
}

int Process::wait()
{
	int status = 0;
	while (true) {
		if (::waitpid(processId_, &status, 0) == -1) {
			if (errno == EINTR) continue;
			PONA_SYSTEM_EXCEPTION;
		}
		break;
	}
	if (WIFEXITED(status))
		status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		status = WTERMSIG(status) + 128;
	return status;
}

void Process::cd(String path)
{
	if (::chdir(path.utf8()) == -1)
		PONA_SYSTEM_EXCEPTION;
}

String Process::cwd()
{
	char* cs = ::getcwd(0, 0x10000);
	if (!cs)
		PONA_SYSTEM_EXCEPTION;
	String path(cs);
	::free(cs);
	return path;
}

String Process::execPath()
{
	String path;
	#ifdef __linux
	CString lnPath = String(Format("/proc/%%/exe") << currentProcessId()).utf8();
	ssize_t bufSize = 1024;
	while (true) {
		char* buf = (char*)::malloc(bufSize + 1);
		::memset(buf, 0, bufSize + 1);
		ssize_t ret = ::readlink(lnPath, buf, bufSize);
		if (ret == -1)
			PONA_SYSTEM_EXCEPTION;
		if (ret < bufSize) {
			path = buf;
			::free(buf);
			break;
		}
		bufSize *= 2;
		::free(buf);
	}
	#endif
	#ifdef __MACH__
	char* buf = 0;
	uint32_t bufSize = 0;
	_NSGetExecutablePath(buf, &bufSize);
	buf = (char*)::malloc(bufSize + 1);
	::memset(buf, 0, bufSize + 1);
	_NSGetExecutablePath(buf, &bufSize);
	path = buf;
	::free(buf);
	#endif
	return path;
}

mode_t Process::setFileCreationMask(mode_t mask) { return ::umask(mask); }

uid_t Process::realUserId() { return ::getuid(); }
gid_t Process::realGroupId() { return ::getgid(); }
uid_t Process::effectiveUserId() { return ::geteuid(); }
gid_t Process::effectiveGroupId() { return ::getegid(); }

bool Process::isSuperUser() { return (::geteuid() == 0) || (::getegid() == 0); }

String Process::env(String key)
{
	return getenv(key.utf8());
}

void Process::setEnv(String key, String value)
{
	if (setenv(key.utf8(), value.utf8(), 1) == -1)
		PONA_SYSTEM_EXCEPTION;
}

void Process::unsetEnv(String key)
{
	errno = 0;
	unsetenv(key.utf8());
	if (errno != 0)
		PONA_SYSTEM_EXCEPTION;
}

typedef Map<String, String> EnvMap;
Ref<EnvMap, Owner> Process::envMap()
{
	char** env = ::environ;
	Ref<EnvMap, Owner> map = new EnvMap;
	int i = 0;
	while (env[i] != 0) {
		String s(env[i]);
		int k = s->find(String("="));
		if (k != s->length()) {
			String key = s->copy(0, k);
			String value = s->copy(k + 1, s->length() - (k + 1));
			map->set(key, value);
		}
		++i;
	}
	return map;
}

pid_t Process::currentProcessId() { return getpid(); }
pid_t Process::parentProcessId() { return getppid(); }

void Process::signal(pid_t processId, int signal)
{
	if (::kill(processId, signal) == -1)
		PONA_SYSTEM_EXCEPTION;
}

void Process::broadcast(pid_t processGroupId, int signal)
{
	if (::kill(-processGroupId, signal) == -1)
		PONA_SYSTEM_EXCEPTION;
}

void Process::raise(int signal)
{
	if (::raise(signal) == -1)
		PONA_SYSTEM_EXCEPTION;
}

void Process::sleep(Time duration)
{
	struct timespec req, rem;
	req.tv_sec = duration.sec();
	req.tv_nsec = duration.nsec();
	rem.tv_sec = 0;
	rem.tv_nsec = 0;
	while (true) {
		if (::nanosleep(&req, &rem) == -1) {
			if (errno == EINTR) {
				if ((rem.tv_sec > 0) || (rem.tv_nsec > 0)) {
					req = rem;
					continue;
				}
			}
			else
				PONA_SYSTEM_EXCEPTION;
		}
		break;
	}
}

void Process::exit(int exitCode)
{
	::exit(exitCode);
}

} // namespace pona
