/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <sys/types.h> // pid_t
#include <cc/files>
#include <cc/Map>
#include <cc/SignalSet>
#include <cc/SystemStream>
#include <cc/exceptions>

namespace cc {

/** \class SubProcess SubProcess.h cc/SubProcess
  * \ingroup concurrency
  * \brief Spawned child process
  */
class SubProcess: public SystemStream
{
public:
    /// Type of process
    enum Type {
        GroupMember,  ///< make sub-process join the current process group
        GroupLeader,  ///< create a new process group and make sub-process the group leader
        SessionLeader ///< create a new session and initial process group and make sub-process the session leader
    };

    /// I/O forwarding policy
    enum Forwarding {
        NoForwarding,    ///< inherit standard I/O streams from parent process
        ForwardBySocket, ///< connect to the child process by a local socket
        ForwardByPts     ///< connect to the child process by a pseudo-terminal
    };

    typedef Map<String, String> EnvMap; ///< environment map type
    typedef Map<int, Ref<SystemStream> > Overloads; ///< I/O overloads map type

    /** \brief Sub-process worker delegate
      */
    class Worker: public Object
    {
    public:
        virtual int run() = 0;
    };

    /** Sub-process staging helper
      */
    class Staging: public Object
    {
    public:
        /** Create a new sub-process creation parameter set
          * \return new object instance
          */
        static Ref<Staging> create() { return new Params(); }

        /** Set the type of the new sub-process
          * \param type process hierarchy type
          * \return pointer to this object
          */
        Staging *setType(Type type) { type_ = type; return this; }

        /** Set the command to execute
          * \param command path to executable and command line parameters
          * \return pointer to this object
          */
        Staging *setCommand(String command) { command_ = command; return this; }

        /** Set I/O forwarding method
          * \param forwarding I/O forwarding method
          * \return pointer to this object
          */
        Staging *setForwarding(Forwarding forwarding) { forwarding_ = forwarding; return this; }

        /** Define which entries in the file descriptor table to overload
          * \param overloads overloading map
          * \return pointer to this object
          * \see cc::StdOutFd, cc::StdErrFd, cc::StdInFd
          */
        Staging *setOverloads(Overloads *overloads) { overloads_ = overloads; return this; }

        /** Set command line arguments
          * \param args argument list, first entry is the executable name
          * \return pointer to this object
          */
        Staging *setArgs(StringList *args) { args_ = args; return this; }

        /** Set environment map
          * \param envMap the environmap to use for the new sub-process (or zero pointer to inherit the parents environment map)
          * \return pointer to this object
          */
        Staging *setEnvMap(EnvMap *envMap) { envMap_ = envMap; return this; }

        /** Set working directory
          * \param workDir initial working directory
          * \return pointer to this object
          */
        Staging *setWorkDir(String workDir) { workDir_ = workDir; return this; }

        /** Set file creation mask
          * \param userMask new file creation mask
          * \return pointer to this object
          */
        Staging *setUserMask(int userMask) { userMask_ = userMask; return this; }

        /** Set the signal mask for the new sub-process
          * \param signalMask signal mask to setup
          * \return pointer to this object
          *
          * If no signal mask is specified (or a null pointer is passed) the signal mask is reset
          * to all unblock for the new sub-process.
          */
        Staging *setSignalMask(SignalSet *signalMask) { signalMask_ = signalMask; return this; }

        /** Set worker object (in case no command/args is specified)
          * \param worker worker to start in sub-process (Worker::run() is invoked)
          * \return pointer to this object
          */
        Staging *setWorker(Worker *worker) { worker_ = worker; return this; }

        /// Finalise staging, start new child process
        inline Ref<SubProcess> start() { return SubProcess::open(this); }

        /// Convenience function, equivalent to start()->wait()
        inline int exec() { return start()->wait(); }

    protected:
        friend class SubProcess;

        Staging():
            type_(SubProcess::GroupLeader),
            forwarding_(ForwardBySocket),
            userMask_(-1)
        {}

        Type type_;
        String command_;
        Forwarding forwarding_;
        Ref<Overloads> overloads_;
        Ref<StringList> args_;
        Ref<EnvMap> envMap_;
        String workDir_;
        int userMask_;
        Ref<SignalSet> signalMask_;
        Ref<Worker> worker_;
    };

    typedef Staging Params;

    static Ref<Staging> stage() {
        Ref<Staging> staging = new Staging;
        staging->forwarding_ = NoForwarding;
        return staging;
    }

    /** \brief Shorthand for SubProcess::Params::create()
      */
    static Ref<Params> params();

    /** \brief Spawn a new child process
      * \param params Sub-process creation parameters
      * \return new object instance
      */
    static Ref<SubProcess> open(Params *params);

    /** \brief Convenience wrapper to SubProcess::open(Params *)
      * \param command program path + arguments (e.g. "ls -l")
      * \param stdErr optional overload for standard error
      * \return new object instance
      */
    static Ref<SubProcess> open(String command, SystemStream *stdErr = 0);

    /** \brief Convenience wrapper to SubProcess::open(Params *)
      * \param worker child process worker
      * \param stdErr optional overload for standard error
      * \return new SubProcess instance
      */
    static Ref<SubProcess> open(SubProcess::Worker *worker, SystemStream *stdErr = 0);

    /** \brief Convenience wrapper to SubProcess::open(Params *) without I/O forwarding
      * \param command program path + arguments (e.g. "ls -l")
      * \return new object instance
      */
    static Ref<SubProcess> execute(String command); // FIXME: slightly confusing; exec() normally waits

    /// Sub-process process ID
    pid_t pid() const;

    /** \brief Send signal to the sub-process
      * \param signal Signal number
      */
    void kill(int signal = SIGTERM);

    /** \brief Kill the entire sub-process group
      * \param signal Signal number
      */
    void killGroup(int signal = SIGTERM);

    /** \brief Wait for sub-process termination
      * \param signalled returns true if the sub-process termination was due to a signal
      * \return exit status of sub-process or signal number if signalled
      */
    int wait(bool *signalled = 0);

private:
    SubProcess(Params *params, pid_t pid = 0);
    ~SubProcess();

    Ref<Params> params_;
    pid_t pid_;
};

} // namespace cc
