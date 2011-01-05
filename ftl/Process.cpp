/*
 * Process.cpp -- child process
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include <sys/wait.h> // waitpid
#include <sys/stat.h> // umask
#include <errno.h> // errno
#include <unistd.h> // chdir, readlink, __MACH__?
#include <stdlib.h> // getenv, setenv, free
#include <time.h> // nanosleep
#include <errno.h> // errno
#ifdef __linux
#include "File.hpp" // to read /proc
#include "Format.hpp"
#endif
#ifdef __MACH__
#include <mach-o/dyld.h> // _NSGetExecutablePath
#include <crt_externs.h> // _NSGetEnviron
#endif
#include "Process.hpp"

#ifndef __MACH__
extern "C" char** environ;
#endif

namespace ftl
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

pid_t Process::id() const { return processId_; }

void Process::kill(int signal, bool* permissionDenied)
{
	if (type() == GroupMember)
		Process::kill(processId_, signal, permissionDenied);
	else
		Process::killGroup(processId_, signal, permissionDenied);
}

bool Process::isRunning() const
{
	int ret = ::kill(processId_, 0);
	if ((ret != 0) && (errno != ESRCH))
		FTL_SYSTEM_EXCEPTION;
	return (ret == 0);
}

int Process::wait()
{
	int status = 0;
	while (true) {
		if (::waitpid(processId_, &status, 0) == -1) {
			if (errno == EINTR) continue;
			FTL_SYSTEM_EXCEPTION;
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
	if (::chdir(path) == -1)
		FTL_SYSTEM_EXCEPTION;
}

String Process::cwd()
{
	int size = 0x1000;
	char* buf = (char*)ftl::malloc(size);
	char* ret = 0;
	while (true) {
		ret = ::getcwd(buf, size);
		if (ret) break;
		if (errno == ERANGE) {
			ftl::free(buf);
			size += 0x1000;
			buf = (char*)ftl::malloc(size);
		}
		else
			FTL_SYSTEM_EXCEPTION;
	}
	String path(ret);
	ftl::free(buf);
	return path;
}

String Process::execPath()
{
	String path;
	#ifdef __linux
	String lnPath = String(Format("/proc/%%/exe") << currentId());
	ssize_t bufSize = 1024;
	while (true) {
		char* buf = (char*)ftl::malloc(bufSize + 1);
		mem::clr(buf, bufSize + 1);
		ssize_t ret = ::readlink(lnPath, buf, bufSize);
		if (ret == -1)
			FTL_SYSTEM_EXCEPTION;
		if (ret < bufSize) {
			path = buf;
			ftl::free(buf);
			break;
		}
		bufSize *= 2;
		ftl::free(buf);
	}
	#endif
	#ifdef __MACH__
	char* buf = 0;
	uint32_t bufSize = 0;
	_NSGetExecutablePath(buf, &bufSize);
	buf = (char*)ftl::malloc(bufSize + 1);
	mem::clr(buf, bufSize + 1);
	_NSGetExecutablePath(buf, &bufSize);
	path = buf;
	ftl::free(buf);
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
	return getenv(key);
}

void Process::setEnv(String key, String value)
{
	if (setenv(key, value, 1) == -1)
		FTL_SYSTEM_EXCEPTION;
}

void Process::unsetEnv(String key)
{
	errno = 0;
	unsetenv(key);
	if (errno != 0)
		FTL_SYSTEM_EXCEPTION;
}

Ref<EnvMap, Owner> Process::envMap()
{
	char** env = environ();
	Ref<EnvMap, Owner> map = new EnvMap;
	int i = 0;
	while (env[i] != 0) {
		Ref<StringList, Owner> parts = String(env[i]).split("=");
		if (parts->length() == 2)
			map->insert(parts->at(0), parts->at(1));
		++i;
	}
	return map;
}

char**& Process::environ()
{
	return
	#ifdef __MACH__
		*_NSGetEnviron();
	#else
		::environ;
	#endif
}

pid_t Process::currentId() { return getpid(); }
pid_t Process::parentId() { return getppid(); }

void Process::kill(pid_t processId, int signal, bool* permissionDenied)
{
	if (::kill(processId, signal) == -1) {
		if ((errno == EPERM) && (permissionDenied))
			*permissionDenied = true;
		else
			FTL_SYSTEM_EXCEPTION;
	}
	else {
		if (permissionDenied)
			*permissionDenied = false;
	}
}

void Process::killGroup(pid_t processGroupId, int signal, bool* permissionDenied)
{
	Process::kill(-processGroupId, signal, permissionDenied);
}

void Process::raise(int signal)
{
	if (::raise(signal) == -1)
		FTL_SYSTEM_EXCEPTION;
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
				FTL_SYSTEM_EXCEPTION;
		}
		break;
	}
}

void Process::exit(int exitCode)
{
	::exit(exitCode);
}

} // namespace ftl
