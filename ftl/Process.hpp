/*
 * Process.hpp -- child process
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_PROCESS_HPP
#define FTL_PROCESS_HPP

#include <sys/types.h> // pid_t
#include <signal.h> // SIGTERM, etc.
#include "atoms"
#include "String.hpp"
#include "Map.hpp"
#include "SystemStream.hpp"
#include "LineSink.hpp"
#include "LineSource.hpp"
#include "Time.hpp"

namespace ftl
{

class ProcessFactory;

typedef Map<String, String> EnvMap;

class Process: public Instance
{
public:
	// -- child process control interface
	
	enum Type {
		GroupMember,
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
	
	int type() const;
	int ioPolicy() const;
	
	Ref<SystemStream> rawInput() const;
	Ref<SystemStream> rawOutput() const;
	Ref<SystemStream> rawError() const;
	
	Ref<LineSink> input() const;
	Ref<LineSource> output() const;
	Ref<LineSource> error() const;
	
	pid_t id() const;
	
	void kill(int signal = SIGTERM, bool* permissionDenied = 0);
	bool isRunning() const;
	
	int wait();
	
	// -- querying the current process status
	
	static void cd(String path);
	static String cwd();
	static String execPath();
	
	static mode_t setFileCreationMask(mode_t mask);
	
	static uid_t realUserId();
	static gid_t realGroupId();
	static uid_t effectiveUserId();
	static gid_t effectiveGroupId();
	static bool isSuperUser();
	
	static String env(String key);
	static void setEnv(String key, String value);
	static void unsetEnv(String key);
	static char**& environ();
	
	static Ref<EnvMap, Owner> envMap();
	
	static pid_t currentId();
	static pid_t parentId();
	
	static void kill(pid_t processId, int signal, bool* permissionDenied = 0);
	static void killGroup(pid_t processGroupId, int signal, bool* permissionDenied = 0);
	static void raise(int signal);
	
	static void sleep(Time duration);
	static void exit(int exitCode);
	
protected:
	friend class ProcessFactory;
	
	Process(
		int type,
		int ioPolicy,
		Ref<SystemStream> rawInput,
		Ref<SystemStream> rawOutput,
		Ref<SystemStream> rawError,
		pid_t processId
	);
	
private:
	int type_;
	int ioPolicy_;
	
	Ref<SystemStream, Owner> rawInput_;
	Ref<SystemStream, Owner> rawOutput_;
	Ref<SystemStream, Owner> rawError_;
	
	Ref<LineSink, Owner> input_;
	Ref<LineSource, Owner> output_;
	Ref<LineSource, Owner> error_;
	
	pid_t processId_;
};

} // namespace ftl

#endif // FTL_PROCESS_HPP
