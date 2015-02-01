/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_PROCESSFACTORY_H
#define FLUX_PROCESSFACTORY_H

#include <flux/String>
#include <flux/SignalSet>
#include <flux/Map>
#include <flux/Process>

namespace flux {

class SystemStream;

/** \brief Child process factory
  * \see ThreadFactory
  */
class ProcessFactory: public Object
{
public:
    inline static Ref<ProcessFactory> create(int type = Process::GroupMember) {
        return new ProcessFactory(type);
    }

    inline String execPath() const { return execPath_; }

    void setExecPath(String path);
    void setArguments(StringList *list);
    void setEnvMap(EnvMap *map);
    void setWorkingDirectory(String path);

    void setIoPolicy(int flags);
    void setIn(SystemStream *stream);
    void setOut(SystemStream *stream);
    void setErr(SystemStream *stream);

    void setSignalMask(SignalSet *mask);
    void setFileCreationMask(int mask);

    String command() const;
    void setCommand(String command);

    Ref<Process> produce();

protected:
    virtual int incarnate();
    ProcessFactory(int type = Process::GroupMember);

private:
    int type_;
    int ioPolicy_;
    String workingDirectory_;

    String execPath_;
    Ref<StringList> arguments_;
    Ref<EnvMap> envMap_;

    Ref<SignalSet> signalMask_;
    int fileCreationMask_;

    String command_;

    Ref<SystemStream> in_;
    Ref<SystemStream> out_;
    Ref<SystemStream> err_;
};

} // namespace flux

#endif // FLUX_PROCESSFACTORY_H
