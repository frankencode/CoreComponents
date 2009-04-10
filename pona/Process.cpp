/*
 * Process.cpp -- process creation, I/O forwarding, signalling
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <sys/types.h> // pid_t, mode_t, uid_t, gid_t
#include <sys/ioctl.h> // ioctl
#include <sys/stat.h> // umask
#include <sys/wait.h> // waitpid
#include <unistd.h> // fork, execve, dup2, chdir, close, environ
#include <fcntl.h> // open
#include <stdlib.h> // exit
#include <signal.h> // kill, raise
#include <termios.h> // tcgetattr, tcsetattr
#include <errno.h> // errno

#include "Exception.hpp"
#include "StandardStreams.hpp"
#include "Process.hpp"

extern "C" char** environ;

namespace pona
{

Process::Process()
	: processType_(SimpleChild),
	  ioPolicy_(0),
	  pid_(-1)
{}

void Process::start()
{
	int inputPipe[2];
	int outputPipe[2];
	int errorPipe[2];
	
	int ptyMaster, ptySlave;
	
	if (ioPolicy_ & ForwardByPseudoTerminal)
	{
		ptyMaster = posix_openpt(O_RDWR|O_NOCTTY);
		if (ptyMaster == -1)
			PONA_SYSTEM_EXCEPTION;
		if (grantpt(ptyMaster) == -1)
			PONA_SYSTEM_EXCEPTION;
		if (unlockpt(ptyMaster) == -1)
			PONA_SYSTEM_EXCEPTION;
	}
	else
	{
		if (ioPolicy_ & ForwardInput)
			if (::pipe(inputPipe) == -1)
				PONA_SYSTEM_EXCEPTION;
		
		if (ioPolicy_ & ForwardOutput)
			if (::pipe(outputPipe) == -1)
				PONA_SYSTEM_EXCEPTION;
		
		if (ioPolicy_ & ForwardError)
			if (::pipe(errorPipe) == -1)
				PONA_SYSTEM_EXCEPTION;
	}
	
	int ret = ::fork();
	
	if (ret == 0)
	{
		// child process
		
		if (processType_ == GroupLeader)
			::setpgid(0, 0);
		else if (processType_ == SessionLeader)
			::setsid();
		
		if (ioPolicy_ & ForwardByPseudoTerminal)
		{
			char* ptySlavePath = ::ptsname(ptyMaster);
			if (!ptySlavePath)
				PONA_SYSTEM_EXCEPTION;
				
			if (::close(ptyMaster) == -1)
				PONA_SYSTEM_EXCEPTION;
			
			ptySlave = ::open(ptySlavePath, O_RDWR|O_NOCTTY);
			if (ptySlave == -1)
				PONA_SYSTEM_EXCEPTION;
			
			{
				struct termios attr;
				::tcgetattr(ptySlave, &attr);
				attr.c_iflag ^= IXON;
			#ifdef IUTF8
				attr.c_iflag |= IUTF8;
			#endif // IUTF8
				::tcsetattr(ptySlave, TCSANOW, &attr);
			}
			
			if (processType_ == SessionLeader)
				if (::ioctl(ptySlave, TIOCSCTTY, 0) == -1)
					PONA_SYSTEM_EXCEPTION;
		}
		
		if (ioPolicy_ & CloseInput) ::close(0);
		if (ioPolicy_ & CloseOutput) ::close(1);
		if (ioPolicy_ & CloseError) ::close(2);
		
		if (workingDirectory_ != String()) {
			if (execPath_->contains(String("/")))
				execPath_ = String() << pona::cwd() << "/" << execPath_;
			if (::chdir(workingDirectory_.utf8()) == -1)
				PONA_SYSTEM_EXCEPTION;
		}
		
		if (rawInput_) ::dup2(rawInput_->fd(), 0);
		if (rawOutput_) ::dup2(rawOutput_->fd(), 1);
		if (rawError_) ::dup2(rawError_->fd(), 2);

		if (ioPolicy_ & ForwardByPseudoTerminal)
		{
			if (ioPolicy_ & ForwardInput) ::dup2(ptySlave, 0);
			if (ioPolicy_ & ForwardOutput) ::dup2(ptySlave, 1);
			if (ioPolicy_ & ForwardError) ::dup2(ptySlave, 2);
			::close(ptySlave);
		}
		else
		{
			if (ioPolicy_ & ForwardInput) {
				::close(inputPipe[1]);
				::dup2(inputPipe[0], 0);
				::close(inputPipe[0]);
			}
			if (ioPolicy_ & ForwardOutput) {
				::close(outputPipe[0]);
				::dup2(outputPipe[1], 1);
				::close(outputPipe[1]);
			}
			if (ioPolicy_ & ForwardError) {
				::close(errorPipe[0]);
				::dup2(errorPipe[1], 2);
				::close(errorPipe[1]);
			}
		}
		
		if (execPath_ != String())
		{
			// prepare the argument list
			
			int argc = 1;
			if (options_)
				argc += options_->length();
				
			char** argv = new char*[argc + 1];
			
			argv[0] = execPath_.strdup();
			if (options_)
				for (int i = 0, n = options_->length(); i < n; ++i)
					argv[i + 1] = options_->get(i).strdup();
			argv[argc] = 0;
			
			// prepare the environment map
			
			char** envp = 0;
			
			if (!envMap_)
			{
				envp = environ;
			}
			else
			{
				Ref<List<EnvMap::Element>, Owner> envList = envMap_->toList();
				int envc = envList->length();
				envp = new char*[envc + 1];

				for (int i = 0, n = envList->length(); i < n; ++i)
					envp[i] = (String() << envList->get(i).key() << "=" << envList->get(i).value()).strdup();
				
				envp[envc] = 0;
			}
			
			// load new program
			
			::execve(execPath_.utf8(), argv, envp);
			
			PONA_SYSTEM_EXCEPTION;
		}
		else
		{
			rawInput_ = pona::rawInput();
			rawOutput_ = pona::rawOutput();
			rawError_ = pona::rawError();
			input_ = pona::input();
			output_ = pona::output();
			error_ = pona::error();
			::exit(run());
		}
	}
	else if (ret > 0)
	{
		// parent process
		
		pid_ = ret;
		rawInput_ = 0;
		rawOutput_ = 0;
		rawError_ = 0;
		
		if (ioPolicy_ & ForwardByPseudoTerminal)
		{
			if (ioPolicy_ & ForwardInput)
				rawInput_ = new SystemStream(ptyMaster);
			if ((ioPolicy_ & ForwardOutput) || (ioPolicy_ & ForwardError))
				rawOutput_ = new SystemStream(ptyMaster);
		}
		else
		{
			if (ioPolicy_ & ForwardInput) {
				::close(inputPipe[0]);
				rawInput_ = new SystemStream(inputPipe[1]);
			}
			if (ioPolicy_ & ForwardOutput) {
				::close(outputPipe[1]);
				rawOutput_ = new SystemStream(outputPipe[0]);
			}
			if (ioPolicy_ & ForwardError) {
				::close(errorPipe[1]);
				rawError_ = new SystemStream(errorPipe[0]);
			}
		}
		
		input_ = 0;
		output_ = 0;
		error_ = 0;
		if (rawInput_) input_ = new LineSink(rawInput_);
		if (rawOutput_) output_ = new LineSource(rawOutput_);
		if (rawError_) error_ = new LineSource(rawError_);
	}
	else if (ret < 0)
	{
		PONA_SYSTEM_EXCEPTION;
	}
}

/** start a daemon child process
  */
void Process::startDaemon()
{
	::umask(0);
	setProcessType(SessionLeader);
	setIoPolicy(CloseInput|CloseOutput|CloseError);
	setWorkingDirectory("/");
}

/** send signal to single process (leaf) or complete process group (job or daemon)
  */
void Process::sendSignal(int signal)
{
	if (::kill((processType_ == SimpleChild) ? pid_ : -pid_, signal) == -1)
		PONA_SYSTEM_EXCEPTION;
}

/** wait for termination of child process
  *
  * \return termination status code
  */
int Process::wait()
{
	int status = 0;
	int ret = 0;
	while (true) {
		ret = ::waitpid(pid_, &status, 0);
		if (ret == -1) {
			if (errno == EINTR) continue;
			PONA_SYSTEM_EXCEPTION;
		}
		break;
	}
	return WEXITSTATUS(status);
}

} // namespace pona
