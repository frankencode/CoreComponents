/*
 * ProcessFactory.hpp -- process creation
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_PROCESSFACTORY_HPP
#define FTL_PROCESSFACTORY_HPP

#include "String.hpp"
#include "SignalSet.hpp"
#include "Map.hpp"

namespace ftl
{

class Process;
class SystemStream;

class ProcessFactory: public Instance
{
public:
	inline static Ref<ProcessFactory, Owner> create() {
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

	Ref<StringList> arguments();
	void setArguments(Ref<StringList> list);

	typedef Map<String, String> EnvMap;
	Ref<EnvMap> envMap();
	void setEnvMap(Ref<EnvMap> map);

	Ref<SignalSet> signalMask();
	void setSignalMask(Ref<SignalSet> mask);

	bool hasFileCreationMask() const;
	void setFileCreationMask(int mask);
	void unsetFileCreationMask();

	Ref<SystemStream> rawInput() const;
	Ref<SystemStream> rawOutput() const;
	Ref<SystemStream> rawError() const;

	void setRawInput(Ref<SystemStream> stream);
	void setRawOutput(Ref<SystemStream> stream);
	void setRawError(Ref<SystemStream> stream);

	void daemonize();

	Ref<Process, Owner> produce();

protected:
	virtual int incarnate();
	ProcessFactory();

private:
	int type_;
	int ioPolicy_;
	String workingDirectory_;

	String execPath_;
	Ref<StringList, Owner> arguments_;
	Ref<EnvMap, Owner> envMap_;

	Ref<SignalSet, Owner> signalMask_;

	bool hasFileCreationMask_;
	int fileCreationMask_;

	Ref<SystemStream, Owner> rawInput_;
	Ref<SystemStream, Owner> rawOutput_;
	Ref<SystemStream, Owner> rawError_;
};

} // namespace ftl

#endif // FTL_PROCESSFACTORY_HPP
