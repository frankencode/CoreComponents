/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_PROCESS_H
#define FLUX_PROCESS_H

#include <sys/types.h> // pid_t
#include <signal.h> // SIGTERM, etc.
#include <flux/Exception>
#include <flux/String>
#include <flux/Map>
#include <flux/SystemStream>
#include <flux/LineSource>

namespace flux {

class ProcessFactory;

typedef Map<String, String> EnvMap;

/** \brief Child process control
  */
class Process: public Stream
{
    // FIXME: split up into Process, CurrentProcess
public:
    static Ref<Process> start(String command, int ioPolicy = 0);
    static Ref<Process> start(String command, ProcessFactory *factory);

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

    SystemStream *in() const;
    SystemStream *out() const;
    SystemStream *err() const;

    LineSource *lineOut();
    LineSource *lineErr();

    pid_t id() const;

    void kill(int signal = SIGTERM, bool *permissionDenied = 0);
    bool isRunning() const;

    int wait();

    // -- stream interface

    virtual bool readyRead(double interval) const;
    virtual int read(ByteArray *data);
    virtual void write(const ByteArray *data);

    // -- query / modify the current process status

    static void cd(String path);
    static String cwd();
    static String execPath();

    static mode_t setFileCreationMask(mode_t mask);

    static uid_t realUserId();
    static gid_t realGroupId();
    static uid_t effectiveUserId();
    static gid_t effectiveGroupId();
    static bool isSuperUser();
    static void setUserId(uid_t uid);
    static void setEffectiveUserId(uid_t uid);

    static String env(String key);
    static void setEnv(String key, String value);
    static void unsetEnv(String key);
    static char **&environ();

    static Ref<EnvMap> envMap();

    static pid_t currentId();
    static pid_t parentId();

    static void kill(pid_t processId, int signal, bool *permissionDenied = 0);
    static void killGroup(pid_t processGroupId, int signal, bool *permissionDenied = 0);
    static void raise(int signal);
    static int alarm(int seconds);

    static void sleep(double duration);
    static void exit(int exitCode);

    static void daemonize();
    static bool isDaemonized();

protected:
    friend class ProcessFactory;

    Process(
        int type,
        int ioPolicy,
        SystemStream *rawInput,
        SystemStream *rawOutput,
        SystemStream *rawError,
        pid_t processId,
        String command
    );

    ~Process();

private:
    static void forwardSignal(int signal);

    int type_;
    int ioPolicy_;

    Ref<SystemStream> in_;
    Ref<SystemStream> out_;
    Ref<SystemStream> err_;

    Ref<LineSource> lineOut_;
    Ref<LineSource> lineErr_;

    pid_t processId_;
    String command_;
};

/** \brief Child process error
  */
class ProcessError: public Exception
{
public:
    ProcessError(int status, String command)
        : status_(status),
          command_(command)
    {}
    ~ProcessError() throw() {}

    inline int status() const { return status_; }
    inline String command() const { return command_; }

    virtual String message() const;

private:
    int status_;
    String command_;
};

} // namespace flux

#endif // FLUX_PROCESS_H
