/*
 * ProcessFactory.hpp -- process creation
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_PROCESSFACTORY_HPP
#define PONA_PROCESSFACTORY_HPP

#include "SignalSet.hpp"
#include "Process.hpp"

namespace pona
{

class ProcessFactory: public Instance
{
public:
	ProcessFactory();
	
	int type() const;
	void setType(int type);
	
	int ioPolicy() const;
	void setIoPolicy(int flags);
	
	UString workingDirectory();
	void setWorkingDirectory(UString path);
	
	UString execPath() const;
	void setExecPath(UString path);
	
	Ref<UStringList> options();
	void setOptions(Ref<UStringList> list);
	
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
	UString workingDirectory_;
	
	UString execPath_;
	Ref<UStringList, Owner> options_;
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
