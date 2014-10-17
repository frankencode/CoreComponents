/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <sys/types.h> // pid_t, mode_t, uid_t, gid_t
#include <sys/ioctl.h> // ioctl
#include <sys/stat.h> // umask
#include <unistd.h> // fork, execve, dup2, chdir, close, environ
#include <fcntl.h> // open
#include <stdlib.h> // exit, posix_openpt, grantpt, unlockpt
#include <termios.h> // tcgetattr, tcsetattr
#include <flux/Format>
#include <flux/File>
#include <flux/exceptions>
#include <flux/ProcessFactory>

namespace flux {

ProcessFactory::ProcessFactory(int type)
	: type_(type),
	  ioPolicy_(0),
	  fileCreationMask_(-1)
{}

void ProcessFactory::setExecPath(String path)
{
	if (path->contains('~')) {
		String home = Process::env("HOME");
		if (home != "") path = path->replace("~", home);
	}
	execPath_ = path;
}

void ProcessFactory::setArguments(StringList *list) { arguments_ = list; }
void ProcessFactory::setEnvMap(EnvMap *map) { envMap_ = map; }
void ProcessFactory::setWorkingDirectory(String path) { workingDirectory_ = path; }

void ProcessFactory::setIoPolicy(int flags) { ioPolicy_ = flags; }
void ProcessFactory::setIn(SystemStream *stream) { in_ = stream; }
void ProcessFactory::setOut(SystemStream *stream) { out_ = stream; }
void ProcessFactory::setErr(SystemStream *stream) { err_ = stream; }

void ProcessFactory::setSignalMask(SignalSet *mask) { signalMask_ = mask; }
void ProcessFactory::setFileCreationMask(int mask) { fileCreationMask_ = mask; }

void ProcessFactory::setCommand(String command)
{
	command_ = command->simplify()->trimInsitu();

	Ref<StringList> args = command_->split(' ');
	setArguments(args);
	String name = args->at(0);

	String path = File::lookup(name);
	if (path == "") path = name;
	setExecPath(path);
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
		if (ptyMaster == -1) FLUX_SYSTEM_DEBUG_ERROR(errno);
		if (::grantpt(ptyMaster) == -1) FLUX_SYSTEM_DEBUG_ERROR(errno);
		if (::unlockpt(ptyMaster) == -1) FLUX_SYSTEM_DEBUG_ERROR(errno);
		ptySlave = ::open(ptsname(ptyMaster), O_RDWR|O_NOCTTY);
		if (ptySlave == -1) FLUX_SYSTEM_DEBUG_ERROR(errno);
	}
	else
	{
		if (ioPolicy_ & Process::ForwardInput)
			if (::pipe(inputPipe) == -1)
				FLUX_SYSTEM_DEBUG_ERROR(errno);

		if (ioPolicy_ & Process::ForwardOutput)
			if (::pipe(outputPipe) == -1)
				FLUX_SYSTEM_DEBUG_ERROR(errno);

		if (ioPolicy_ & Process::ForwardError)
			if (::pipe(errorPipe) == -1)
				FLUX_SYSTEM_DEBUG_ERROR(errno);
	}

	char **argv = 0, **envp = 0;

	if (execPath_ != "")
	{
		// prepare the argument list

		StringList *arguments = arguments_;
		if (arguments) if (arguments->count() == 0) arguments = 0;

		int argc = arguments ? arguments->count() : 1;
		argv = new char*[argc + 1];

		if (arguments) {
			for (int i = 0; i < arguments->count(); ++i)
				argv[i] = strdup(arguments->at(i)->chars());
		}
		else {
			argv[0] = strdup(execPath_->chars());
		}
		argv[argc] = 0;

		// prepare the environment map

		envp = 0;

		if (envMap_) {
			int n = envMap_->count();
			envp = new char*[n + 1];
			for (int i = 0; i < n; ++i)
				envp[i] = strdup(String(Format() << envMap_->keyAt(i) <<  "=" << envMap_->valueAt(i))->chars());
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
				FLUX_SYSTEM_DEBUG_ERROR(errno);

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
				FLUX_SYSTEM_DEBUG_ERROR(errno);
		}

		if (signalMask_) {
			if (::sigprocmask(SIG_SETMASK, signalMask_->rawSet(), 0) == -1)
				FLUX_SYSTEM_DEBUG_ERROR(errno);
		}

		if (fileCreationMask_ >= 0) ::umask(fileCreationMask_);

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

			FLUX_SYSTEM_DEBUG_ERROR(errno);
		}
		else
		{
			::exit(incarnate());
		}
	}
	else if (ret > 0)
	{
		// parent process

		if (argv) {
			for (int i = 0; argv[i]; ++i)
				flux::free(argv[i]);
			delete[] argv;
		}
		if (envMap_) {
			for (int i = 0; envp[i]; ++i)
				flux::free(envp[i]);
			delete[] envp;
		}

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

		return new Process(type_, ioPolicy_, in, out, err, ret, command_);
	}
	else if (ret < 0)
	{
		FLUX_SYSTEM_DEBUG_ERROR(errno);
	}

	FLUX_ASSERT(0 == 1);
	return 0;
}

int ProcessFactory::incarnate() { return 0; }

} // namespace flux
