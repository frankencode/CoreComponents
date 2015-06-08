/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
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
#ifdef __linux
#include <flux/File> // to read /proc
#endif
#ifdef __MACH__
#include <mach-o/dyld.h> // _NSGetExecutablePath
#include <crt_externs.h> // _NSGetEnviron
#endif
#include <flux/exceptions>
#include <flux/Format>
#include <flux/Thread>
#include <flux/ProcessFactory>
#include <flux/Process>

#ifndef __MACH__
extern "C" char **environ;
#endif

namespace flux {

Ref<Process> Process::start(String command, int ioPolicy)
{
    Ref<ProcessFactory> factory = ProcessFactory::create();
    factory->setIoPolicy(ioPolicy);
    factory->setSignalMask(defaultSignalMask());
    return start(command, factory);
}

Ref<Process> Process::start(String command, ProcessFactory *factory)
{
    factory->setCommand(command);
    return factory->produce();
}

Process::Process(
    int type,
    int ioPolicy,
    SystemStream *in,
    SystemStream *out,
    SystemStream *err,
    pid_t processId,
    String command
)
    : type_(type),
      ioPolicy_(ioPolicy),
      in_(in),
      out_(out),
      err_(err),
      processId_(processId),
      command_(command)
{}

Process::~Process()
{
    if (processId_ != -1) {
        int status = 0;
        while (true) {
            if (::waitpid(processId_, &status, 0) == -1) {
                if (errno == EINTR) continue;
                if (errno == ECHILD) break;
                FLUX_SYSTEM_DEBUG_ERROR(errno);
            }
            break;
        }
        if (status != 0)
            throw ProcessError(status, command_);
    }
}

int Process::type() const { return type_; }
int Process::ioPolicy() const { return ioPolicy_; }

SystemStream *Process::in() const { return in_; }
SystemStream *Process::out() const { return out_; }
SystemStream *Process::err() const { return err_; }

LineSource *Process::lineOut() {
    if (!lineOut_ && out_) lineOut_ = LineSource::open(out_);
    return lineOut_;
}

LineSource *Process::lineErr() {
    if (!lineErr_ && err_) lineErr_ = LineSource::open(err_);
    return lineErr_;
}

pid_t Process::id() const { return processId_; }

void Process::kill(int signal, bool *permissionDenied)
{
    if (type() == GroupMember)
        Process::kill(processId_, signal, permissionDenied);
    else
        Process::killGroup(processId_, signal, permissionDenied);
}

bool Process::isRunning() const
{
    int ret = ::kill(processId_, 0);
    if ((ret != 0) && (errno != ESRCH))
        FLUX_SYSTEM_DEBUG_ERROR(errno);
    return (ret == 0);
}

/*! wait for until child process terminates
  * \ret exit status of child
  */
int Process::wait()
{
    int status = 0;
    if (::waitpid(processId_, &status, 0) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
    if (WIFEXITED(status))
        status = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        status = WTERMSIG(status) + 128;
    processId_ = -1;
    return status;
}

bool Process::readyRead(double interval) const
{
    return out()->readyRead(interval);
}

int Process::read(ByteArray *data)
{
    return out()->read(data);
}

void Process::write(const ByteArray *data)
{
    in()->write(data);
}

void Process::cd(String path)
{
    if (::chdir(path) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
}

String Process::cwd()
{
    int size = 0x1000;
    char *buf = (char *)flux::malloc(size);
    char *ret = 0;
    while (true) {
        ret = ::getcwd(buf, size);
        if (ret) break;
        if (errno == ERANGE) {
            flux::free(buf);
            size += 0x1000;
            buf = (char *)flux::malloc(size);
        }
        else
            FLUX_SYSTEM_DEBUG_ERROR(errno);
    }
    String path(ret);
    flux::free(buf);
    return path;
}

String Process::execPath()
{
    String path;
    #ifdef __linux
    String linkPath = Format("/proc/%%/exe") << currentId();
    path = File::readlink(linkPath);
    #endif
    #ifdef __MACH__
    char *buf = 0;
    uint32_t bufSize = 0;
    _NSGetExecutablePath(buf, &bufSize);
    buf = (char *)flux::malloc(bufSize + 1);
    memclr(buf, bufSize + 1);
    _NSGetExecutablePath(buf, &bufSize);
    path = buf;
    flux::free(buf);
    #endif
    return path;
}

mode_t Process::setFileCreationMask(mode_t mask) { return ::umask(mask); }

uid_t Process::realUserId() { return ::getuid(); }
gid_t Process::realGroupId() { return ::getgid(); }
uid_t Process::effectiveUserId() { return ::geteuid(); }
gid_t Process::effectiveGroupId() { return ::getegid(); }

bool Process::isSuperUser() { return (::geteuid() == 0) || (::getegid() == 0); }

void Process::setUserId(uid_t uid)
{
    if (::setuid(uid) == -1) FLUX_SYSTEM_DEBUG_ERROR(errno);
}

void Process::setEffectiveUserId(uid_t uid)
{
    if (::seteuid(uid) == -1) FLUX_SYSTEM_DEBUG_ERROR(errno);
}

String Process::env(String key)
{
    return getenv(key);
}

void Process::setEnv(String key, String value)
{
    if (setenv(key, value, 1) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
}

void Process::unsetEnv(String key)
{
    errno = 0;
    unsetenv(key);
    if (errno != 0)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
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
            FLUX_SYSTEM_DEBUG_ERROR(errno);
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
        FLUX_SYSTEM_DEBUG_ERROR(errno);
}

int Process::alarm(int seconds)
{
    int ret = ::alarm(seconds);
    if (ret == -1) FLUX_SYSTEM_DEBUG_ERROR(errno);
    return ret;
}

/** Returns the default signal mask of this process before any modifications
  * by Thread::blockSignals() or Thread::unblockSignals()
  */
SignalSet *Process::defaultSignalMask()
{
    return Singleton<SignalSet>::instance();
}

void Process::forwardSignal(int signal)
{
    Thread::self()->lastSignal_ = signal;
    Thread::self()->handleSignal(signal);
}

void Process::sleep(double duration)
{
    struct timespec req, rem;

    double sec = 0;
    req.tv_nsec = modf(duration, &sec) * 1e9;
    req.tv_sec = sec;
    rem.tv_nsec = 0;
    rem.tv_sec = 0;
    while (true) {
        if (::nanosleep(&req, &rem) == -1) {
            if (errno == EINTR) {
                if ((rem.tv_sec > 0) || (rem.tv_nsec > 0)) {
                    req = rem;
                    continue;
                }
            }
            else
                FLUX_SYSTEM_DEBUG_ERROR(errno);
        }
        break;
    }
}

void Process::exit(int exitCode)
{
    ::exit(exitCode);
}

void Process::daemonize()
{
    pid_t pid = ::fork();
    if (pid == -1) FLUX_SYSTEM_DEBUG_ERROR(errno);
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

String ProcessError::message() const
{
    return Format("Process unsuccessful: %% (status = %%)") << command_ << status_;
}

} // namespace flux
