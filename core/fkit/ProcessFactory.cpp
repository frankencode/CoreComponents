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
#include <stdlib.h> // exit, posix_openpt, grantpt, unlockpt
#include <termios.h> // tcgetattr, tcsetattr
#include "Format.h"
#include "File.h"
#include "ProcessFactory.h"

namespace fkit
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

StringList *ProcessFactory::arguments() { return (arguments_) ? arguments_ : arguments_ = StringList::create(); }
void ProcessFactory::setArguments(StringList *list) { arguments_ = list; }

EnvMap *ProcessFactory::envMap() { return (envMap_) ? envMap_ : envMap_ = Process::envMap(); }
void ProcessFactory::setEnvMap(EnvMap *map) { envMap_ = map; }

SignalSet *ProcessFactory::signalMask() { return (signalMask_) ? signalMask_ : signalMask_ = SignalSet::createEmpty(); }
void ProcessFactory::setSignalMask(SignalSet *mask) { signalMask_ = mask; }

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

	Ref<StringList> args = command->split(' ');
	setArguments(args);
	String name = args->at(0);

	String path = File::lookup(name);
	if (path == "") path = name;
	setExecPath(path);
}

SystemStream *ProcessFactory::in() const { return in_; }
SystemStream *ProcessFactory::out() const { return out_; }
SystemStream *ProcessFactory::err() const { return err_; }

void ProcessFactory::setIn(SystemStream *stream) { in_ = stream; }
void ProcessFactory::setOut(SystemStream *stream) { out_ = stream; }
void ProcessFactory::setErr(SystemStream *stream) { err_ = stream; }

void ProcessFactory::daemonize()
{
	setFileCreationMask(0);
	setType(Process::SessionLeader);
	setIoPolicy(Process::CloseInput|Process::CloseOutput|Process::CloseError);
	setWorkingDirectory("/");
}

Ref<Process> ProcessFactory::produce()
{
	int inputPipe[2];
	int outputPipe[2];
	int errorPipe[2];

	int ptyMaster = -1, ptySlave = -1;

	if (ioPolicy_ & Process::ForwardByPseudoTerminal)
	{
		ptyMaster = ::posix_openpt(O_RDWR|O_NOCTTY);
		if (ptyMaster == -1) FKIT_SYSTEM_EXCEPTION;
		if (::grantpt(ptyMaster) == -1) FKIT_SYSTEM_EXCEPTION;
		if (::unlockpt(ptyMaster) == -1) FKIT_SYSTEM_EXCEPTION;
		ptySlave = ::open(ptsname(ptyMaster), O_RDWR|O_NOCTTY);
		if (ptySlave == -1) FKIT_SYSTEM_EXCEPTION;
	}
	else
	{
		if (ioPolicy_ & Process::ForwardInput)
			if (::pipe(inputPipe) == -1)
				FKIT_SYSTEM_EXCEPTION;

		if (ioPolicy_ & Process::ForwardOutput)
			if (::pipe(outputPipe) == -1)
				FKIT_SYSTEM_EXCEPTION;

		if (ioPolicy_ & Process::ForwardError)
			if (::pipe(errorPipe) == -1)
				FKIT_SYSTEM_EXCEPTION;
	}

	char **argv = 0, **envp = 0;

	if (execPath_ != "")
	{
		// prepare the argument list

		StringList *arguments = arguments_;
		if (arguments) if (arguments->length() == 0) arguments = 0;

		int argc = arguments ? arguments->length() : 1;
		argv = new char*[argc + 1];

		if (arguments) {
			for (int i = 0; i < arguments->length(); ++i)
				argv[i] = strdup(arguments->at(i)->data());
		}
		else {
			argv[0] = strdup(execPath_->data());
		}
		argv[argc] = 0;

		// prepare the environment map

		envp = 0;

		if (envMap_) {
			int n = envMap_->length();
			envp = new char*[n + 1];
			for (int i = 0; i < n; ++i)
				envp[i] = strdup(String(Format() << envMap_->get(i).key() <<  "=" << envMap_->get(i).value())->data());
			envp[n] = 0;
		}
		else {
			envp = Process::environ();
		}
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
				FKIT_SYSTEM_EXCEPTION;

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
				FKIT_SYSTEM_EXCEPTION;
		}

		if (signalMask_) {
			if (::sigprocmask(SIG_SETMASK, signalMask_->rawSet(), 0) == -1)
				FKIT_SYSTEM_EXCEPTION;
		}

		if (hasFileCreationMask_) ::umask(fileCreationMask_);

		if (ioPolicy_ & Process::CloseInput) ::close(0);
		if (ioPolicy_ & Process::CloseOutput) ::close(1);
		if (ioPolicy_ & Process::CloseError) ::close(2);

		if (in_) ::dup2(in_->fd(), 0);
		if (out_) ::dup2(out_->fd(), 1);
		if (err_) ::dup2(err_->fd(), 2);

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
			}
			if (ioPolicy_ & Process::ForwardOutput) {
				::close(outputPipe[0]);
				::dup2(outputPipe[1], 1);
			}
			if (ioPolicy_ & Process::ForwardError) {
				::close(errorPipe[0]);
				::dup2(errorPipe[1], 2);
			}
		}

		if (ioPolicy_ & Process::ErrorToOutput) ::dup2(1, 2);

		if (execPath_ != "")
		{
			// load new program

			::execve(execPath_, argv, envp);

			FKIT_SYSTEM_EXCEPTION;
		}
		else
		{
			::exit(incarnate());
		}
	}
	else if (ret > 0)
	{
		// parent process

		Ref<SystemStream> in = in_;
		Ref<SystemStream> out = out_;
		Ref<SystemStream> err = err_;

		if (ioPolicy_ & Process::ForwardByPseudoTerminal)
		{
			::close(ptySlave);

			if (ioPolicy_ & Process::ForwardInput)
				in = SystemStream::create(ptyMaster);
			if ((ioPolicy_ & Process::ForwardOutput) || (ioPolicy_ & Process::ForwardError)) {
				if (in)
					out = in;
				else
					out = SystemStream::create(ptyMaster);
			}
		}
		else
		{
			if (ioPolicy_ & Process::ForwardInput) {
				::close(inputPipe[0]);
				in = SystemStream::create(inputPipe[1]);
			}
			if (ioPolicy_ & Process::ForwardOutput) {
				::close(outputPipe[1]);
				out = SystemStream::create(outputPipe[0]);
			}
			if (ioPolicy_ & Process::ForwardError) {
				::close(errorPipe[1]);
				err = SystemStream::create(errorPipe[0]);
			}
		}

		return new Process(type_, ioPolicy_, in, out, err, ret);
	}
	else if (ret < 0)
	{
		FKIT_SYSTEM_EXCEPTION;
	}

	FKIT_ASSERT(0 == 1);
	return 0;
}

int ProcessFactory::incarnate() { return 0; }

} // namespace fkit
