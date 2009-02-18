/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_PROCESS_HPP
#define PONA_PROCESS_HPP

#ifdef PONA_POSIX
#include <sys/types.h>
#else // if PONA_WINDOWS
typedef DWORD pid_t;
#endif

#include "Atoms.hpp"
#include "String.hpp"
#include "List.hpp"
#include "Map.hpp"
#include "SystemStream.hpp"
#include "LineSink.hpp"
#include "LineSource.hpp"
#include "ProcessStatus.hpp"

namespace pona
{

/** \brief Process
  *
  * Overview:
  * - "processType" ... type of process to start
  *        A simple child process is assumed to have no further children.
  *        If a group leader receives a termination signal (SIGTERM or SIGKILL), the signal will
  *        be forwarded to all its simple children recursively.
  *        A session leader is a group leader, which is not controlled by any terminal
  *        and therefore does not receive control signals like SIGHUB.
  * - "ioPolicy" ... defines which standard I/O streams should be forwarded to the parent
  * - "workingDirectory" ... working directory of the child process
  * - "execPath" ... path of executable file to load (script or binary)
  * - "argList" ... argument list to be passed to the executable
  * - "envMap" ... environment variables to be passed to the executable
  * - "rawInput" ... standard input stream of the child process
  * - "rawOutput" ... standard output stream of the child process
  * - "rawError" ... standard error stream of the child process
  *
  * LIMITATIONS FOR WINDOWS (NT, XP and newer versions):
  *   1) An execPath has to be passed and Process::run() will never be entered.
  *      In other words you can't create worker clones. This limitation originates
  *      form the lack of a fork(2) system call under Windows.
  *   2) The method isSuperUser() returns always true.
  *      Just type in your favourite search engine "ntdll exploit" or "Windows rootkit"
  *      to learn more about how to implement MyProcess::makeRoot().
  *   3) Windows has no concept of friendly process termination and therefore
  *      terminate() has the same effect than directly calling kill().
  *   4) There is no support for session leadership implemented.
  *      This means startDaemon() is not working and setting process type to
  *      sessionLeader has no effect.
  *   5) You cannot start a new process with an empty environment.
  *      (Windows exposes here fancy undocumented behavior.)
  *   6) TcpStreams are no SystemStreams under Windows, so you can't simply pass
  *      TCP streams to a child process. This is a limitation of the OS,
  *      which is known for years and will probably never be fixed.
  *   7) On Windows there is no support for pseudo-terminals.
  *      (Afaik Windows supports only screen visible "consoles".)
  *
  * \todo
  *   - String lookupPath(String name)
  */
class Process: public Instance
{
public:
	typedef List<String> ArgList;
	typedef Map<String, String> EnvMap;
	
	enum ProcessType {
		simpleChild,
		groupLeader,
		sessionLeader
	};
	
	enum IoPolicy {
		forwardInput = 1,
		forwardOutput = 2,
		forwardError = 4,
		forwardByPseudoTerminal = 8,
		closeInput = 16,
		closeOutput = 32,
		closeError = 64,
		forwardAll = forwardInput|forwardOutput|forwardError,
		closeAll = closeInput|closeOutput|closeError
	};
	
	Process();
	
	// ---------------------------------------------------------------
	// pre-start parameterization interface
	// ---------------------------------------------------------------
	
	int processType() const { return processType_; }
	int ioPolicy() const { return ioPolicy_; }
	String workingDirectory() const { return workingDirectory_; }
	void setProcessType(int type) { processType_ = type; }
	void setIoPolicy(int flags) { ioPolicy_ = flags; }
	void setWorkingDirectory(String path) { workingDirectory_ = path; }
	
	String execPath() const { return execPath_; }
	Ref<ArgList> arguments() { return (argList_) ? argList_ : argList_ = new ArgList; }
	Ref<EnvMap> environment() { return (envMap_) ? envMap_ : envMap_ = ProcessStatus::environment(); }
	void setExecPath(String path) { execPath_ = path; }
	void setArguments(Ref<ArgList> list) { argList_ = list; }
	void setEnvironment(Ref<EnvMap> map) { envMap_ = map; }
	
	void setRawInput(Ref<SystemStream> stream) { rawInput_ = stream; }
	void setRawOutput(Ref<SystemStream> stream) { rawOutput_ = stream; }
	void setRawError(Ref<SystemStream> stream) { rawError_ = stream; }
	
	void start();
	void startDaemon();
	
	// ---------------------------------------------------------------
	// post-start control and I/O interface
	// ---------------------------------------------------------------
	
	inline Ref<SystemStream> rawInput() const { return rawInput_; }
	inline Ref<SystemStream> rawOutput() const { return rawOutput_; }
	inline Ref<SystemStream> rawError() const { return rawError_; }
	
	inline Ref<LineSink> input() { return input_; }
	inline Ref<LineSource> output() { return output_; }
	inline Ref<LineSource> error() { return error_; }
	
	pid_t pid() const { return pid_; }
	
	void terminate();
	void hangup();
	void kill();
	int wait();
	
protected:
	virtual int run() { return 0; }
	
private:
	int processType_;
	int ioPolicy_;
	String workingDirectory_;
	
	String execPath_;
	Ref<ArgList, Owner> argList_;
	Ref<EnvMap, Owner> envMap_;
	
	Ref<SystemStream, Owner> rawInput_;
	Ref<SystemStream, Owner> rawOutput_;
	Ref<SystemStream, Owner> rawError_;
	
	Ref<LineSink, Owner> input_;
	Ref<LineSource, Owner> output_;
	Ref<LineSource, Owner> error_;
	
	pid_t pid_;
	
#ifdef PONA_WINDOWS
	void* handle_;
#endif
};

} // namespace pona

#endif // PONA_PROCESS_HPP

