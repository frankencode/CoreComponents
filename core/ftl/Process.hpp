 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_PROCESS_HPP
#define FTL_PROCESS_HPP

#include <sys/types.h> // pid_t
#include <signal.h> // SIGTERM, etc.
#include "string.hpp"
#include "Map.hpp"
#include "SystemStream.hpp"
#include "LineSink.hpp"
#include "LineSource.hpp"
#include "Time.hpp"

namespace ftl
{

class ProcessFactory;

typedef Map<string, string> EnvMap;

FTL_STD_EXCEPTION(ProcessException);

class Process: public Instance
{
public:
	static hook<Process> start(string command, int ioPolicy = 0);
	static hook<Process> start(string command, ProcessFactory *factory);

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

	SystemStream *rawInput() const;
	SystemStream *rawOutput() const;
	SystemStream *rawError() const;

	LineSink *input() const;
	LineSource *output() const;
	LineSource *error() const;

	pid_t id() const;

	void kill(int signal = SIGTERM, bool *permissionDenied = 0);
	bool isRunning() const;

	int wait();

	// -- query / modify the current process status

	static void cd(string path);
	static string cwd();
	static string execPath();

	static mode_t setFileCreationMask(mode_t mask);

	static uid_t realUserId();
	static gid_t realGroupId();
	static uid_t effectiveUserId();
	static gid_t effectiveGroupId();
	static bool isSuperUser();

	static string env(string key);
	static void setEnv(string key, string value);
	static void unsetEnv(string key);
	static char **&environ();

	static hook<EnvMap> envMap();

	static pid_t currentId();
	static pid_t parentId();

	static void kill(pid_t processId, int signal, bool *permissionDenied = 0);
	static void killGroup(pid_t processGroupId, int signal, bool *permissionDenied = 0);
	static void raise(int signal);
	static int alarm(int seconds);

	static void enableInterrupt(int signal, bool on = true);

	static void sleep(Time duration);
	static void exit(int exitCode);

protected:
	friend class ProcessFactory;

	Process(
		int type,
		int ioPolicy,
		SystemStream *rawInput,
		SystemStream *rawOutput,
		SystemStream *rawError,
		pid_t processId
	);

	~Process();

private:
	static void forwardSignal(int signal);

	int type_;
	int ioPolicy_;

	hook<SystemStream> rawInput_;
	hook<SystemStream> rawOutput_;
	hook<SystemStream> rawError_;

	hook<LineSink> input_;
	hook<LineSource> output_;
	hook<LineSource> error_;

	pid_t processId_;
};

} // namespace ftl

#endif // FTL_PROCESS_HPP
