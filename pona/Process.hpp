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

#include <sys/types.h> // pid_t
#include "atoms"
#include "context.hpp"
#include "SystemStream.hpp"
#include "LineSink.hpp"
#include "LineSource.hpp"

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
  * \todo
  *   - String lookupPath(String name)
  */
class Process: public Instance
{
public:
	enum ProcessType {
		SimpleChild,
		GroupLeader,
		SessionLeader
	};
	
	enum IoPolicy {
		ForwardInput = 1,
		ForwardOutput = 2,
		ForwardError = 4,
		ForwardByPseudoTerminal = 8,
		CloseInput = 16,
		CloseOutput = 32,
		CloseError = 64,
		ForwardAll = ForwardInput|ForwardOutput|ForwardError,
		CloseAll = CloseInput|CloseOutput|CloseError
	};
	
	Process();
	
	// ---------------------------------------------------------------
	// pre-start parameterization interface
	// ---------------------------------------------------------------
	
	int processType() const { return processType_; }
	void setProcessType(int type) { processType_ = type; }
	
	int ioPolicy() const { return ioPolicy_; }
	void setIoPolicy(int flags) { ioPolicy_ = flags; }
	
	String workingDirectory() const { return workingDirectory_; }
	void setWorkingDirectory(String path) { workingDirectory_ = path; }
	
	String execPath() const { return execPath_; }
	void setExecPath(String path) { execPath_ = path; }
	
	Ref<StringList> options() { return (options_) ? options_ : options_ = new StringList; }
	void setOptions(Ref<StringList> list) { options_ = list; }
	
	Ref<EnvMap> envMap() { return (envMap_) ? envMap_ : envMap_ = pona::envMap(); }
	void setEnvMap(Ref<EnvMap> map) { envMap_ = map; }
	
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
	Ref<StringList, Owner> options_;
	Ref<EnvMap, Owner> envMap_;
	
	Ref<SystemStream, Owner> rawInput_;
	Ref<SystemStream, Owner> rawOutput_;
	Ref<SystemStream, Owner> rawError_;
	
	Ref<LineSink, Owner> input_;
	Ref<LineSource, Owner> output_;
	Ref<LineSource, Owner> error_;
	
	pid_t pid_;
};

} // namespace pona

#endif // PONA_PROCESS_HPP

