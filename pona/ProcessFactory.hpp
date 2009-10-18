/*
 * ProcessFactory.hpp -- process creation
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#ifndef PONA_PROCESSFACTORY_HPP
#define PONA_PROCESSFACTORY_HPP

#include "SignalSet.hpp"
#include "Process.hpp"

namespace pona
{

class PONA_API ProcessFactory: public Instance
{
public:
	ProcessFactory();
	
	int type() const;
	void setType(int type);
	
	int ioPolicy() const;
	void setIoPolicy(int flags);
	
	String workingDirectory();
	void setWorkingDirectory(String path);
	
	String execPath() const;
	void setExecPath(String path);
	
	Ref<StringList> options();
	void setOptions(Ref<StringList> list);
	
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
	
private:
	int type_;
	int ioPolicy_;
	String workingDirectory_;
	
	String execPath_;
	Ref<StringList, Owner> options_;
	Ref<EnvMap, Owner> envMap_;
	
	Ref<SignalSet, Owner> signalMask_;
	
	bool hasFileCreationMask_;
	int fileCreationMask_;
	
	Ref<SystemStream, Owner> rawInput_;
	Ref<SystemStream, Owner> rawOutput_;
	Ref<SystemStream, Owner> rawError_;
};

} // namespace pona

#endif // PONA_PROCESSFACTORY_HPP
