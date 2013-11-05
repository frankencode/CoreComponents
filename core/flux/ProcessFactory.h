/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_PROCESSFACTORY_H
#define FLUX_PROCESSFACTORY_H

#include "String.h"
#include "SignalSet.h"
#include "Map.h"
#include "Process.h"

namespace flux
{

class SystemStream;

class ProcessFactory: public Object
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

	SystemStream *in() const;
	SystemStream *out() const;
	SystemStream *err() const;

	void setIn(SystemStream *stream);
	void setOut(SystemStream *stream);
	void setErr(SystemStream *stream);

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

	Ref<SystemStream> in_;
	Ref<SystemStream> out_;
	Ref<SystemStream> err_;
};

} // namespace flux

#endif // FLUX_PROCESSFACTORY_H
