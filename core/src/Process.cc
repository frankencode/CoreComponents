/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Process>
#include <cc/File>
#include <cc/strings> // cc::strdup, cc::free
#include <sys/wait.h> // waitpid
#include <sys/stat.h> // umask
#include <errno.h> // errno
#include <unistd.h> // chdir, alarm, __MACH__?
#include <stdlib.h> // getenv, setenv, free
#ifdef __MACH__
#include <mach-o/dyld.h> // _NSGetExecutablePath
#include <crt_externs.h> // _NSGetEnviron
#endif

#ifndef __MACH__
extern "C" char **environ;
#endif

namespace cc {

Process::Instance::Instance(Command &command):
    Process::Instance::Instance{command.instance_}
{}

Process::Instance::Instance(Command::Instance *command)
{
    /// locate executable and prepare argument list

    String execPath;
    StringList args = command->args_;

    if (command->command_ != "") {
        String cmd = command->command_;
        if (cmd->startsWith(' ') || cmd->endsWith(' '))
            cmd = cmd->trim();
        if (!cmd->contains(' ')) execPath = cmd;
        if (args->count() == 0) args = cmd->simplify()->split(' ');
    }

    if (execPath == "" && args->count() > 0) execPath = args->at(0);

    if (execPath != "") {
        if (execPath->contains('/')) {
            if (!File::checkAccess(execPath, FileAccess::Execute)) throw CommandNotFound{execPath};
        }
        else {
            String path = File::locate(execPath, Process::getEnv("PATH")->split(':'), FileAccess::Execute);
            if (path == "") throw CommandNotFound{execPath};
            execPath = path;
        }
    }

    /// prepare argument list and environment map

    char **argv = nullptr;
    char **envp = nullptr;
    {
        int argc = (args->count() > 0) ? args->count() : 1;
        argv = new char*[argc + 1];

        if (args->count() > 0) {
            for (int i = 0; i < args->count(); ++i)
                argv[i] = cc::strdup(args->at(i)->chars());
        }
        else {
            argv[0] = cc::strdup(execPath->chars());
        }
        argv[argc] = 0;

        if (command->customEnvMap_) {
            const EnvMap &envMap = command->envMap_;
            const int n = envMap->count();
            envp = new char*[n + 1];
            int i = 0;
            for (auto &pair: envMap) {
                envp[i] = cc::strcat(pair->key()->chars(), "=", pair->value()->chars());
                ++i;
            }
            envp[n] = nullptr;
        }
    }

    /// spawn new child process

    #ifdef __GLIBC__
    #if __GLIBC_PREREQ(2, 29)
    #else
    class CwdGuard {
    public:
        CwdGuard(const String &cwd):
            cwdSaved_{cwd != "" ? Process::getWorkingDirectory() : String{}}
        {
            if (cwd != "") Process::setWorkingDirectory(cwd);
        }
        ~CwdGuard()
        {
            if (cwdSaved_ != "") Process::setWorkingDirectory(cwdSaved_);
        }
    private:
        String cwdSaved_;
    };
    CwdGuard guard{command->cwd_};
    #endif
    #endif

    pid_ = -1;
    int ret = ::posix_spawn(&pid_, execPath, &command->fileActions_, &command->spawnAttributes_, argv, envp ? envp : ::environ);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);

    if (envp) {
        for (int i = 0; envp[i]; ++i)
            cc::free(envp[i]);
        delete[] envp;
    }

    groupLead_ = command->groupLead_;

    for (int i = 0; i <= 2; ++i)
        standardStreams_[i] = command->standardStreams_[i];

    command->startDone();
}

Process::Instance::~Instance()
{
    wait();
}

pid_t Process::Instance::id() const
{
    return pid_;
}

void Process::Instance::kill(SystemSignal signal)
{
    if (::kill(groupLead_ ? -pid_ : pid_, +signal) == -1) {
        if (errno == EPERM) throw PermissionError{};
        CC_SYSTEM_DEBUG_ERROR(errno);
    }
}

int Process::Instance::wait()
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

SystemStream Process::Instance::input() const
{
    return standardStreams_[0];
}

SystemStream Process::Instance::output() const
{
    return standardStreams_[1];
}

SystemStream Process::Instance::error() const
{
    return standardStreams_[2];
}

void Process::setWorkingDirectory(const String &path)
{
    if (::chdir(path) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

String Process::getWorkingDirectory()
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
    String path{ret};
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

void Process::setUserMask(FileMode newMask, FileMode *oldMask)
{
    auto h = ::umask(+newMask);
    if (oldMask) *oldMask = static_cast<FileMode>(h);
}

uid_t Process::getRealUserId() { return ::getuid(); }
gid_t Process::getRealGroupId() { return ::getgid(); }
uid_t Process::getEffectiveUserId() { return ::geteuid(); }
gid_t Process::getEffectiveGroupId() { return ::getegid(); }

bool Process::isSuperUser() { return (::geteuid() == 0) || (::getegid() == 0); }

void Process::setUserId(uid_t uid)
{
    if (::setuid(uid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::setGroupId(gid_t gid)
{
    if (::setgid(gid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::setEffectiveUserId(uid_t uid)
{
    if (::seteuid(uid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::setEffectiveGroupId(gid_t gid)
{
    if (::setegid(gid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

String Process::getEnv(const String &key)
{
    return ::getenv(key);
}

String Process::getEnv(const String &name, const String &defaultValue)
{
    String value = ::getenv(name);
    if (value == "") value = defaultValue;
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

EnvMap Process::getEnvMap()
{
    char **env = environ();
    EnvMap map;
    for (int i = 0; env[i]; ++i) {
        String s{env[i]};
        int j = 0;
        if (s->find('=', &j)) {
            String value = s->copy(j + 1, s->count());
            mutate(s)->truncate(j);
            map->insert(s, value);
        }
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

pid_t Process::getId()
{
    return ::getpid();
}

pid_t Process::getParentId()
{
    return ::getppid();
}

void Process::kill(pid_t processId, SystemSignal signal)
{
    if (::kill(processId, +signal) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::killGroup(pid_t processGroupId, SystemSignal signal)
{
    Process::kill(-processGroupId, signal);
}

void Process::raise(SystemSignal signal)
{
    if (::raise(+signal) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
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

} // namesp
