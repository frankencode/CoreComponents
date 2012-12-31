/*
 * Process.cpp -- child process
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <sys/wait.h> // waitpid
#include <sys/stat.h> // umask
#include <errno.h> // errno
#include <unistd.h> // chdir, __MACH__?
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
#include "ProcessFactory.hpp"
#include "Process.hpp"

#ifndef __MACH__
extern "C" char **environ;
#endif

namespace ftl
{

Ref<Process, Owner> Process::start(String command, int ioPolicy)
{
	Ref<ProcessFactory, Owner> factory = ProcessFactory::create();
	factory->setIoPolicy(ioPolicy);
	return start(command, factory);
}

Ref<Process, Owner> Process::start(String command, Ref<ProcessFactory> factory)
{
	Ref<StringList, Owner> args = command->split(' ');
	factory->setArguments(args);
	String name = args->at(0);

	String path = File::lookup(name);
	if (path == "") path = name;
	factory->setExecPath(path);

	return factory->produce();
}

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
	if (rawInput) input_ = LineSink::open(rawInput);
	if (rawOutput) output_ = LineSource::open(rawOutput);
	if (rawError) error_ = LineSource::open(rawError);
}

Process::~Process()
{
	if (processId_ != -1) {
		int status = 0;
		while (true) {
			if (::waitpid(processId_, &status, 0) == -1) {
				if (errno == EINTR) continue;
				if (errno == ECHILD) break;
				FTL_SYSTEM_EXCEPTION;
			}
			break;
		}
		if (status != 0)
			throw ProcessException(Format("Process unsuccessful, status = %%") << status);
	}
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

void Process::kill(int signal, bool *permissionDenied)
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

/*! wait for until child process terminates
  * Throws Interrupt exception, if the calling process received a signal while waiting.
  * \ret exit status of child
  */
int Process::wait()
{
	int status = 0;
	if (::waitpid(processId_, &status, 0) == -1) {
		if (errno == EINTR) throw Interrupt();
		FTL_SYSTEM_EXCEPTION;
	}
	if (WIFEXITED(status))
		status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		status = WTERMSIG(status) + 128;
	processId_ = -1;
	return status;
}

/*! Suspend execution of the calling process until one of its children terminates.
  * Throws Interrupt exception, if the calling process received a signal while waiting.
  * \ret exist status of child
  */
pid_t Process::wait(int *status)
{
	int h;
	pid_t pid = ::waitpid(-1, &h, 0);
	if (pid == -1) {
		if (errno == EINTR) throw Interrupt();
		FTL_SYSTEM_EXCEPTION;
	}
	if (WIFEXITED(h))
		h = WEXITSTATUS(h);
	else if (WIFSIGNALED(h))
		h = WTERMSIG(h) + 128;
	if (status) *status = h;
	return pid;
}

void Process::cd(String path)
{
	if (::chdir(path) == -1)
		FTL_SYSTEM_EXCEPTION;
}

String Process::cwd()
{
	int size = 0x1000;
	char *buf = (char *)mem::alloc(size);
	char *ret = 0;
	while (true) {
		ret = ::getcwd(buf, size);
		if (ret) break;
		if (errno == ERANGE) {
			mem::free(buf);
			size += 0x1000;
			buf = (char *)mem::alloc(size);
		}
		else
			FTL_SYSTEM_EXCEPTION;
	}
	String path(ret);
	mem::free(buf);
	return path;
}

String Process::execPath()
{
	String path;
	#ifdef __linux
	String linkPath = Format("/proc/%%/exe") << currentId();
	path = File::readlink(linkPath);
	#endif
	#ifdef __MACH__
	char *buf = 0;
	uint32_t bufSize = 0;
	_NSGetExecutablePath(buf, &bufSize);
	buf = (char *)mem::alloc(bufSize + 1);
	mem::clr(buf, bufSize + 1);
	_NSGetExecutablePath(buf, &bufSize);
	path = buf;
	mem::free(buf);
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
	char **env = environ();
	Ref<EnvMap, Owner> map = EnvMap::create();
	int i = 0;
	while (env[i] != 0) {
		Ref<StringList, Owner> parts = String(env[i])->split("=");
		if (parts->length() == 2)
			map->insert(parts->at(0), parts->at(1));
		++i;
	}
	return map;
}

char **&Process::environ()
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

void Process::kill(pid_t processId, int signal, bool *permissionDenied)
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

void Process::killGroup(pid_t processGroupId, int signal, bool *permissionDenied)
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
