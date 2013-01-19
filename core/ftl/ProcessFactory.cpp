 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include <sys/types.h> // pid_t, mode_t, uid_t, gid_t
#include <sys/ioctl.h> // ioctl
#include <sys/stat.h> // umask
#include <unistd.h> // fork, execve, dup2, chdir, close, environ
#include <fcntl.h> // open
#include <stdlib.h> // exit
#include <termios.h> // tcgetattr, tcsetattr
#if __linux
#include <pty.h>
#else
#include <util.h>
#endif
#include "Format.hpp"
#include "File.hpp"
#include "ProcessFactory.hpp"

namespace ftl
{

ProcessFactory::ProcessFactory()
	: type_(Process::GroupMember),
	  ioPolicy_(0),
	  hasFileCreationMask_(false)
{}

int ProcessFactory::type() const { return type_; }
void ProcessFactory::setType(int type) { type_ = type; }

int ProcessFactory::ioPolicy() const { return ioPolicy_; }
void ProcessFactory::setIoPolicy(int flags) { ioPolicy_ = flags; }

/** working directory of new process
  */
String ProcessFactory::workingDirectory() { return (workingDirectory_ != "") ? workingDirectory_ : workingDirectory_ = Process::cwd(); }
void ProcessFactory::setWorkingDirectory(String path) { workingDirectory_ = path; }

/** relative or absolute path to executable image or script
  */
String ProcessFactory::execPath() const { return execPath_; }
void ProcessFactory::setExecPath(String path) { execPath_ = path; }

Ref<StringList> ProcessFactory::arguments() { return (arguments_) ? arguments_ : arguments_ = StringList::create(); }
void ProcessFactory::setArguments(Ref<StringList> list) { arguments_ = list; }

Ref<EnvMap> ProcessFactory::envMap() { return (envMap_) ? envMap_ : envMap_ = Process::envMap(); }
void ProcessFactory::setEnvMap(Ref<EnvMap> map) { envMap_ = map; }

Ref<SignalSet> ProcessFactory::signalMask() { return (signalMask_) ? signalMask_ : signalMask_ = SignalSet::createEmpty(); }
void ProcessFactory::setSignalMask(Ref<SignalSet> mask) { signalMask_ = mask; }

bool ProcessFactory::hasFileCreationMask() const { return hasFileCreationMask_; }
void ProcessFactory::setFileCreationMask(int mask) {
	fileCreationMask_ = mask;
	hasFileCreationMask_ = true;
}
void ProcessFactory::unsetFileCreationMask() { hasFileCreationMask_ = false; }

String ProcessFactory::command() const { return command_; }

void ProcessFactory::setCommand(String command)
{
	command_ = command;

	Ref<StringList, Owner> args = command->split(' ');
	setArguments(args);
	String name = args->at(0);

	String path = File::lookup(name);
	if (path == "") path = name;
	setExecPath(path);
}

Ref<SystemStream> ProcessFactory::rawInput() const { return rawInput_; }
Ref<SystemStream> ProcessFactory::rawOutput() const { return rawOutput_; }
Ref<SystemStream> ProcessFactory::rawError() const { return rawError_; }

void ProcessFactory::setRawInput(Ref<SystemStream> stream) { rawInput_ = stream; }
void ProcessFactory::setRawOutput(Ref<SystemStream> stream) { rawOutput_ = stream; }
void ProcessFactory::setRawError(Ref<SystemStream> stream) { rawError_ = stream; }

void ProcessFactory::daemonize()
{
	setFileCreationMask(0);
	setType(Process::SessionLeader);
	setIoPolicy(Process::CloseInput|Process::CloseOutput|Process::CloseError);
	setWorkingDirectory("/");
}

Ref<Process, Owner> ProcessFactory::produce()
{
	int inputPipe[2];
	int outputPipe[2];
	int errorPipe[2];

	int ptyMaster = -1, ptySlave = -1;

	if (ioPolicy_ & Process::ForwardByPseudoTerminal)
	{
		if (::openpty(&ptyMaster, &ptySlave, 0/*ptyname*/, 0/*termp*/, 0/*winp*/) == -1)
			FTL_SYSTEM_EXCEPTION;
	}
	else
	{
		if (ioPolicy_ & Process::ForwardInput)
			if (::pipe(inputPipe) == -1)
				FTL_SYSTEM_EXCEPTION;

		if (ioPolicy_ & Process::ForwardOutput)
			if (::pipe(outputPipe) == -1)
				FTL_SYSTEM_EXCEPTION;

		if (ioPolicy_ & Process::ForwardError)
			if (::pipe(errorPipe) == -1)
				FTL_SYSTEM_EXCEPTION;
	}

	int ret = ::fork();

	if (ret == 0)
	{
		// child process

		if (type_ == Process::GroupLeader)
			::setpgid(0, 0);
		else if (type_ == Process::SessionLeader)
			::setsid();

		if (ioPolicy_ & Process::ForwardByPseudoTerminal)
		{
			if (::close(ptyMaster) == -1)
				FTL_SYSTEM_EXCEPTION;

			if (type_ == Process::SessionLeader)
				::ioctl(ptySlave, TIOCSCTTY, 0);

			{
				struct termios attr;
				::tcgetattr(ptySlave, &attr);
				attr.c_iflag ^= IXON;
			#ifdef IUTF8
				attr.c_iflag |= IUTF8;
			#endif // IUTF8
				::tcsetattr(ptySlave, TCSANOW, &attr);
			}
		}

		if (workingDirectory_ != "") {
			if (::chdir(workingDirectory_) == -1)
				FTL_SYSTEM_EXCEPTION;
		}

		if (signalMask_) {
			if (::sigprocmask(SIG_SETMASK, signalMask_, 0) == -1)
				FTL_SYSTEM_EXCEPTION;
		}

		if (hasFileCreationMask_) ::umask(fileCreationMask_);

		if (ioPolicy_ & Process::CloseInput) ::close(0);
		if (ioPolicy_ & Process::CloseOutput) ::close(1);
		if (ioPolicy_ & Process::CloseError) ::close(2);

		if (rawInput_) ::dup2(rawInput_->fd(), 0);
		if (rawOutput_) ::dup2(rawOutput_->fd(), 1);
		if (rawError_) ::dup2(rawError_->fd(), 2);

		if (ioPolicy_ & Process::ForwardByPseudoTerminal)
		{
			if (ioPolicy_ & Process::ForwardInput) ::dup2(ptySlave, 0);
			if (ioPolicy_ & Process::ForwardOutput) ::dup2(ptySlave, 1);
			if (ioPolicy_ & Process::ForwardError) ::dup2(ptySlave, 2);
			::close(ptySlave);
		}
		else
		{
			if (ioPolicy_ & Process::ForwardInput) {
				::close(inputPipe[1]);
				::dup2(inputPipe[0], 0);
				::close(inputPipe[0]);
			}
			if (ioPolicy_ & Process::ForwardOutput) {
				::close(outputPipe[0]);
				::dup2(outputPipe[1], 1);
				::close(outputPipe[1]);
			}
			if (ioPolicy_ & Process::ForwardError) {
				::close(errorPipe[0]);
				::dup2(errorPipe[1], 2);
				::close(errorPipe[1]);
			}
		}

		if (ioPolicy_ & Process::ErrorToOutput) ::dup2(1, 2);

		if (execPath_ != "")
		{
			// prepare the argument list

			Ref<StringList> arguments = arguments_;
			if (arguments) if (arguments->length() == 0) arguments = 0;

			int argc = arguments ? arguments->length() : 1;
			char **argv = new char*[argc + 1];

			if (arguments) {
				for (int i = 0; i < arguments->length(); ++i)
					argv[i] = str::dup(arguments->at(i)->data());
			}
			else {
				argv[0] = str::dup(execPath_->data());
			}
			argv[argc] = 0;

			// prepare the environment map

			char **envp = 0;

			if (envMap_) {
				int n = envMap_->length();
				envp = new char*[n + 1];
				for (int i = 0; i < n; ++i)
					envp[i] = str::dup(String(Format() << envMap_->get(i).key() <<  "=" << envMap_->get(i).value())->data());
				envp[n] = 0;
			}
			else {
				envp = Process::environ();
			}

			// load new program

			::execve(execPath_, argv, envp);

			FTL_SYSTEM_EXCEPTION;
		}
		else
		{
			::exit(incarnate());
		}
	}
	else if (ret > 0)
	{
		// parent process

		Ref<SystemStream, Owner> rawInput = rawInput_;
		Ref<SystemStream, Owner> rawOutput = rawOutput_;
		Ref<SystemStream, Owner> rawError = rawError_;

		if (ioPolicy_ & Process::ForwardByPseudoTerminal)
		{
			::close(ptySlave);

			if (ioPolicy_ & Process::ForwardInput)
				rawInput = SystemStream::create(ptyMaster);
			if ((ioPolicy_ & Process::ForwardOutput) || (ioPolicy_ & Process::ForwardError)) {
				if (rawInput)
					rawOutput = rawInput;
				else
					rawOutput = SystemStream::create(ptyMaster);
			}
		}
		else
		{
			if (ioPolicy_ & Process::ForwardInput) {
				::close(inputPipe[0]);
				rawInput = SystemStream::create(inputPipe[1]);
			}
			if (ioPolicy_ & Process::ForwardOutput) {
				::close(outputPipe[1]);
				rawOutput = SystemStream::create(outputPipe[0]);
			}
			if (ioPolicy_ & Process::ForwardError) {
				::close(errorPipe[1]);
				rawError = SystemStream::create(errorPipe[0]);
			}
		}

		return new Process(
			type_,
			ioPolicy_,
			rawInput,
			rawOutput,
			rawError,
			ret
		);
	}
	else if (ret < 0)
	{
		FTL_SYSTEM_EXCEPTION;
	}

	FTL_ASSERT(0 == 1);
	return 0;
}

int ProcessFactory::incarnate() { return 0; }

} // namespace ftl
