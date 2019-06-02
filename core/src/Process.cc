/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/wait.h> // waitpid
#include <sys/stat.h> // umask
#include <errno.h> // errno
#include <unistd.h> // chdir, alarm, __MACH__?
#include <math.h> // modf
#include <stdlib.h> // getenv, setenv, free
#include <time.h> // nanosleep
#include <errno.h> // errno
#include <string.h> // strcmp
#ifdef __MACH__
#include <mach-o/dyld.h> // _NSGetExecutablePath
#include <crt_externs.h> // _NSGetEnviron
#else
#include <cc/File> // to read /proc
#endif
#include <cc/stdio>
#include <cc/exceptions>
#include <cc/Format>
#include <cc/Thread>
#include <cc/Process>

#ifndef __MACH__
extern "C" char **environ;
#endif

namespace cc {

void Process::cd(String path)
{
    if (::chdir(path) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

String Process::cwd()
{
    int size = 0x1000;
    char *buf = (char *)cc::malloc(size);
    char *ret = 0;
    while (true) {
        ret = ::getcwd(buf, size);
        if (ret) break;
        if (errno == ERANGE) {
            cc::free(buf);
            size += 0x1000;
            buf = (char *)cc::malloc(size);
        }
        else
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
    String path(ret);
    cc::free(buf);
    return path;
}

String Process::exePath()
{
    String path;
    #ifdef __linux
    path = File::readlink("/proc/self/exe");
    #elif __OpenBSD__
    return File::readlink("/proc/curproc/file");
    #elif __NetBSD__
	return File::readlink("/proc/curproc/exe")
    #elif __MACH__
    char *buf = 0;
    uint32_t bufSize = 0;
    _NSGetExecutablePath(buf, &bufSize);
    buf = (char *)cc::malloc(bufSize + 1);
    memclr(buf, bufSize + 1);
    _NSGetExecutablePath(buf, &bufSize);
    path = buf;
    cc::free(buf);
    #else
    #error "Missing implementation for Process::execPath()"
    #endif
    return path;
}

mode_t Process::setUserMask(mode_t mask) { return ::umask(mask); }

uid_t Process::realUserId() { return ::getuid(); }
gid_t Process::realGroupId() { return ::getgid(); }
uid_t Process::effectiveUserId() { return ::geteuid(); }
gid_t Process::effectiveGroupId() { return ::getegid(); }

bool Process::isSuperUser() { return (::geteuid() == 0) || (::getegid() == 0); }

void Process::setUserId(uid_t uid)
{
    if (::setuid(uid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::setEffectiveUserId(uid_t uid)
{
    if (::seteuid(uid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::setPersona(uid_t uid, gid_t gid)
{
    if (::setgid(gid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    if (::setuid(uid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

String Process::env(String key)
{
    return getenv(key);
}

String Process::env(String key, String defaultValue)
{
    String value = getenv(key);
    if (value == "") value = defaultValue;
    return value;
}

void Process::setEnv(String key, String value)
{
    if (setenv(key, value, 1) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::unsetEnv(String key)
{
    errno = 0;
    unsetenv(key);
    if (errno != 0)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

Ref<EnvMap> Process::envMap()
{
    char **env = environ();
    Ref<EnvMap> map = EnvMap::create();
    int i = 0;
    while (env[i] != 0) {
        Ref<StringList> parts = String(env[i])->split("=");
        if (parts->count() == 2)
            map->insert(parts->at(0), parts->at(1));
        ++i;
    }
    return map;
}

char **&Process::environ()
{
    return
    #ifdef __MACH__
        *_NSGetEnviron();
    #else
        ::environ;
    #endif
}

pid_t Process::currentId() { return getpid(); }
pid_t Process::parentId() { return getppid(); }

void Process::kill(pid_t processId, int signal, bool *permissionDenied)
{
    if (::kill(processId, signal) == -1) {
        if ((errno == EPERM) && (permissionDenied))
            *permissionDenied = true;
        else
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
    else {
        if (permissionDenied)
            *permissionDenied = false;
    }
}

void Process::killGroup(pid_t processGroupId, int signal, bool *permissionDenied)
{
    Process::kill(-processGroupId, signal, permissionDenied);
}

void Process::raise(int signal)
{
    if (::raise(signal) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

SignalSet *Process::defaultSignalMask()
{
    return Singleton<SignalSet>::instance();
}

void Process::exit(int exitCode)
{
    ::exit(exitCode);
}

void Process::daemonize()
{
    pid_t pid = ::fork();
    if (pid == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    if (pid != 0) ::exit(0);
    ::setsid();
    ::umask(0);
    ::close(0);
    ::close(1);
    ::close(2);
}

bool Process::isDaemonized()
{
    return ::getsid(0) == ::getpgid(0) && ::isatty(0) != 1;
}

pid_t Process::makeSessionLeader()
{
    return ::setpgid(0, 0);
}

} // namespace cc
