/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <sys/types.h> // pid_t
#include <cc/SignalSet>
#include <cc/String>
#include <cc/Map>
#include <cc/SystemStream>
#include <cc/LineSource>

namespace cc {

typedef Map<String, String> EnvMap;

/** \class Process Process.h cc/Process
  * \brief %Process status and control
  */
class Process
{
public:
    /** Change the current working directory of this process
      * \param path new directory path to enter
      */
    static void cd(String path);

    /// Current working directory of this process
    static String cwd();

    /// Actual pathname of the executed command
    static String exePath();

    static mode_t setUserMask(mode_t mask);

    static uid_t realUserId();
    static gid_t realGroupId();
    static uid_t effectiveUserId();
    static gid_t effectiveGroupId();
    static bool isSuperUser();
    static void setUserId(uid_t uid);
    static void setEffectiveUserId(uid_t uid);
    static void setPersona(uid_t uid, gid_t gid);

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

    /** Returns the default signal mask of this process before any modifications
      * by Thread::blockSignals() or Thread::unblockSignals()
      */
    static SignalSet *defaultSignalMask();

    /** terminate this process
      * \param exitCode exit code (0..255)
      */
    static void exit(int exitCode);

    /// Daemonize this process
    static void daemonize();

    /// Returns true if this process is daemonized
    static bool isDaemonized();
};

} // namespace cc
