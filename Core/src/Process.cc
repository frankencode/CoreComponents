/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Process>
#include <cc/Command>
#include <cc/File>
#include <sys/wait.h> // waitpid
#include <sys/stat.h> // umask
#include <errno.h> // errno
#include <unistd.h> // chdir, alarm, __MACH__?
#include <stdlib.h> // getenv, setenv, malloc, free
#ifdef __MACH__
#include <mach-o/dyld.h> // _NSGetExecutablePath
#include <crt_externs.h> // _NSGetEnviron
#endif

#ifndef __MACH__
extern "C" char **environ;
#endif

namespace cc {

Process::State::~State()
{
    wait();
}

int Process::State::wait()
{
    if (pid_ < 0) return exitStatus_;

    int ret = 0;
    if (::waitpid(pid_, &ret, 0) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    pid_ = -1;

    if (WIFSIGNALED(ret))
        return exitStatus_ = -WTERMSIG(ret);

    return exitStatus_ = WEXITSTATUS(ret);
}

Process::Process(const String &command):
    Process{Command{command}}
{}

Process::Process(const Command &command):
    Object{new State}
{
    Command{command}.start(*this);
}

int Process::exec(const String &command)
{
    Process process {
        Command{command}
        .io(0, IoStream::input())
        .io(1, IoStream::output())
        .io(2, IoStream::error())
    };

    return process.wait();
}

int Process::exec(const List<String> &args)
{
    Process process {
        Command{args}
        .io(0, IoStream::input())
        .io(1, IoStream::output())
        .io(2, IoStream::error())
    };

    return process.wait();
}

int Process::id() const
{
    return me().pid_;
}

int Process::wait()
{
    return me().wait();
}

IoStream Process::input() const
{
    return me().standardStreams_[0];
}

IoStream Process::output() const
{
    return me().standardStreams_[1];
}

IoStream Process::error() const
{
    return me().standardStreams_[2];
}

void Process::kill(int pid, Signal signal)
{
    if (::kill(pid, +signal) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::killGroup(int gid, Signal signal)
{
    if (::kill(-gid, +signal) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::raise(Signal signal)
{
    if (::raise(+signal) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::exit(int status)
{
    ::exit(status);
}

int Process::currentId()
{
    return ::getpid();
}

int Process::parentId()
{
    return ::getppid();
}

String Process::execPath()
{
    String path;
    #if defined __linux || defined __CYGWIN32__ || defined __CYGWIN__
    path = File::readlink("/proc/self/exe");
    #elif __OpenBSD__
    path = File::readlink("/proc/curproc/file");
    #elif __NetBSD__
    path = File::readlink("/proc/curproc/exe")
    #elif __MACH__
    char *buf = 0;
    uint32_t bufSize = 0;
    _NSGetExecutablePath(buf, &bufSize);
    buf = (char *)::malloc(bufSize + 1);
    memset(buf, 0, bufSize + 1);
    _NSGetExecutablePath(buf, &bufSize);
    path = buf;
    ::free(buf);
    #else
    #warning "Missing implementation for Process::execPath()"
    #endif
    return path;
}

String Process::env(const String &name)
{
    return ::getenv(name);
}

String Process::env(const String &name, const String &fallback)
{
    String value = ::getenv(name);
    if (value == "") value = fallback;
    return value;
}

void Process::setEnv(const String &name, const String &value)
{
    if (::setenv(name, value, 1) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::unsetEnv(const String &name)
{
    if (::unsetenv(name) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

Map<String> Process::envMap()
{
    char **env = environ();
    Map<String> map;
    for (int i = 0; env[i]; ++i) {
        String s{env[i]};
        long j = 0;
        if (s.find('=', &j)) {
            String value = s.select(j + 1, s.count());
            s.truncate(j);
            map.insert(s, value);
        }
    }
    return map;
}

String Process::cwd()
{
    int size = 0x1000;
    char *buf = (char *)::malloc(size);
    char *ret = 0;
    while (true) {
        ret = ::getcwd(buf, size);
        if (ret) break;
        if (errno == ERANGE) {
            ::free(buf);
            size += 0x1000;
            buf = (char *)::malloc(size);
        }
        else
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
    String path{ret};
    ::free(buf);
    return path;
}

void Process::cd(const String &path)
{
    if (::chdir(path) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::setUserMask(FileMode newMask, Out<FileMode> oldMask)
{
    auto h = ::umask(+newMask);
    oldMask << static_cast<FileMode>(h);
}

int Process::realUserId()
{
    return ::getuid();
}

int Process::realGroupId()
{
    return ::getgid();
}

int Process::effectiveUserId()
{
    return ::geteuid();
}

int Process::effectiveGroupId()
{
    return ::getegid();
}

bool Process::isSuperUser()
{
    return ::geteuid() == 0 || ::getegid() == 0;
}

void Process::setUserId(int uid)
{
    if (::setuid(uid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::setGroupId(int gid)
{
    if (::setgid(gid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::setEffectiveUserId(int uid)
{
    if (::seteuid(uid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::setEffectiveGroupId(int gid)
{
    if (::setegid(gid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
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

void Process::startSession()
{
    if (::setpgid(0, 0) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
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

} // namespace cc
