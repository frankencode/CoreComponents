 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_PROCESSFACTORY_HPP
#define FTL_PROCESSFACTORY_HPP

#include "String.hpp"
#include "SignalSet.hpp"
#include "Map.hpp"
#include "Process.hpp"

namespace ftl
{

class SystemStream;

class ProcessFactory: public Instance
{
public:
	inline static Ref<ProcessFactory> create() {
		return new ProcessFactory;
	}

	int type() const;
	void setType(int type);

	int ioPolicy() const;
	void setIoPolicy(int flags);

	String workingDirectory();
	void setWorkingDirectory(String path);

	String execPath() const;
	void setExecPath(String path);

	StringList *arguments();
	void setArguments(StringList *list);

	typedef Map<String, String> EnvMap;
	EnvMap *envMap();
	void setEnvMap(EnvMap *map);

	SignalSet *signalMask();
	void setSignalMask(SignalSet *mask);

	bool hasFileCreationMask() const;
	void setFileCreationMask(int mask);
	void unsetFileCreationMask();

	String command() const;
	void setCommand(String command);

	SystemStream *rawInput() const;
	SystemStream *rawOutput() const;
	SystemStream *rawError() const;

	void setRawInput(SystemStream *stream);
	void setRawOutput(SystemStream *stream);
	void setRawError(SystemStream *stream);

	void daemonize();

	Ref<Process> produce();

protected:
	virtual int incarnate();
	ProcessFactory();

private:
	int type_;
	int ioPolicy_;
	String workingDirectory_;

	String execPath_;
	Ref<StringList> arguments_;
	Ref<EnvMap> envMap_;

	Ref<SignalSet> signalMask_;

	bool hasFileCreationMask_;
	int fileCreationMask_;

	String command_;

	Ref<SystemStream> rawInput_;
	Ref<SystemStream> rawOutput_;
	Ref<SystemStream> rawError_;
};

} // namespace ftl

#endif // FTL_PROCESSFACTORY_HPP
