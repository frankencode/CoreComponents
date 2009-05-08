/*
 * Process.hpp -- process creation, I/O forwarding, signalling
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_PROCESS_HPP
#define PONA_PROCESS_HPP

#include <sys/types.h> // pid_t
#include <signal.h> // SIGTERM, etc.
#include "atom"
#include "context.hpp"
#include "SystemStream.hpp"
#include "LineSink.hpp"
#include "LineSource.hpp"

namespace pona
{

class Process: public Instance
{
public:
	enum Type {
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
		ErrorToOutput = 128,
		ForwardAll = ForwardInput|ForwardOutput|ForwardError,
		CloseAll = CloseInput|CloseOutput|CloseError
	};
	
	Process();
	
	// ---------------------------------------------------------------
	// pre-start parameterization interface
	// ---------------------------------------------------------------
	
	int type() const { return type_; }
	void setType(int type) { type_ = type; }
	
	int ioPolicy() const { return ioPolicy_; }
	void setIoPolicy(int flags) { ioPolicy_ = flags; }
	
	String workingDirectory() const { return workingDirectory_; }
	void setWorkingDirectory(String path) { workingDirectory_ = path; }
	
	sigset_t* signalMask() { return &signalMask_; }
	
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
	
	void sendSignal(int signal);
	int wait();
	
protected:
	virtual int run() { return 0; }
	
private:
	int type_;
	int ioPolicy_;
	String workingDirectory_;
	
	sigset_t signalMask_;
	
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

