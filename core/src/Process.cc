/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/wait.h> // waitpid
#include <sys/stat.h> // umask
#include <errno.h> // errno
#include <unistd.h> // chdir, alarm, __MACH__?
#include <stdlib.h> // getenv, setenv, free
#ifdef __MACH__
#include <mach-o/dyld.h> // _NSGetExecutablePath
#include <crt_externs.h> // _NSGetEnviron
#endif
#include <cc/strings> // cc::strdup, cc::free
#include <cc/File>
#include <cc/InputPipe>
#include <cc/OutputPipe>
#include <cc/Process>
#include <cc/Process>

#ifndef __MACH__
extern "C" char **environ;
#endif

namespace cc {

#define CC_SPAWN_CALL(call) \
{\
    int ret = call; \
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret); \
}

Process::Staging::Staging(const String &command):
    command_{command}
{
    CC_SPAWN_CALL(posix_spawnattr_init(&spawnAttributes_));
    CC_SPAWN_CALL(posix_spawn_file_actions_init(&fileActions_));
}

Process::Staging::~Staging()
{
    posix_spawn_file_actions_destroy(&fileActions_);
    posix_spawnattr_destroy(&spawnAttributes_);
}

void Process::Staging::enableSpawnFlag(short flag)
{
    short flags = 0;
    CC_SPAWN_CALL(posix_spawnattr_getflags(&spawnAttributes_, &flags));
    flags |= flag;
    CC_SPAWN_CALL(posix_spawnattr_setflags(&spawnAttributes_, flags));
}

Process::Staging *Process::Staging::setArgs(const StringList *args)
{
    args_ = args;
    return this;
}

Process::Staging *Process::Staging::setEnvMap(const EnvMap *envMap)
{
    envMap_ = envMap;
    return this;
}

Process::Staging *Process::Staging::setProcessGroup(pid_t groupId)
{
    CC_SPAWN_CALL(posix_spawnattr_setpgroup(&spawnAttributes_, groupId));
    enableSpawnFlag(POSIX_SPAWN_SETPGROUP);
    groupLead_ = (groupId == 0);
    return this;
}

Process::Staging *Process::Staging::setSignalDefault(const SignalSet *set)
{
    CC_SPAWN_CALL(posix_spawnattr_setsigdefault(&spawnAttributes_, set->rawSet()));
    enableSpawnFlag(POSIX_SPAWN_SETSIGDEF);
    return this;
}

Process::Staging *Process::Staging::setSignalMask(const SignalSet *mask)
{
    CC_SPAWN_CALL(posix_spawnattr_setsigmask(&spawnAttributes_, mask->rawSet()));
    enableSpawnFlag(POSIX_SPAWN_SETSIGMASK);
    return this;
}

Process::Staging *Process::Staging::setWorkingDirectory(const String &path)
{
    CC_SPAWN_CALL(posix_spawn_file_actions_addchdir_np(&fileActions_, path));
    return this;
}

Process::Staging *Process::Staging::setInputChannel(IoChannel *channel)
{
    return attachChannel(channel, 0);
}

Process::Staging *Process::Staging::setOutputChannel(IoChannel *channel)
{
    return attachChannel(channel, 1);
}

Process::Staging *Process::Staging::setErrorChannel(IoChannel *channel)
{
    return attachChannel(channel, 2);
}

Process::Staging *Process::Staging::attachChannel(IoChannel *channel, int targetFd)
{
    if (!channel->next_) {
        channel->next_ = channelHead_;
        channelHead_ = channel;
    }
    if (0 <= targetFd && targetFd <= 2) standardStreams_[targetFd] = channel;
    CC_SPAWN_CALL(posix_spawn_file_actions_adddup2(&fileActions_, channel->slaveFd(), targetFd));
    return this;
}

Process::Staging *Process::Staging::setInput(SystemStream *stream)
{
    return attach(stream, 0);
}

Process::Staging *Process::Staging::setOutput(SystemStream *stream)
{
    return attach(stream, 1);
}

Process::Staging *Process::Staging::setError(SystemStream *stream)
{
    return attach(stream, 2);
}

Process::Staging *Process::Staging::attach(SystemStream *stream, int targetFd)
{
    if (0 <= targetFd && targetFd <= 2) standardStreams_[targetFd] = stream;
    CC_SPAWN_CALL(posix_spawn_file_actions_adddup2(&fileActions_, stream->fd(), targetFd));
    return this;
}

Ref<Process> Process::Staging::start()
{
    auto spawn = Process::bootstrap(this);

    while (channelHead_) {
        channelHead_->onStart();
        Ref<IoChannel> next = channelHead_->next_;
        channelHead_->next_ = nullptr;
        channelHead_ = next;
    }

    return spawn;
}

Ref<Process> Process::Staging::open()
{
    if (!standardStreams_[0]) setInputChannel(InputPipe::create());

    if (!standardStreams_[1] || !standardStreams_[2]) {
        auto channel = OutputPipe::create();
        if (!standardStreams_[1]) setOutputChannel(channel);
        if (!standardStreams_[2]) setErrorChannel(channel);
    }

    return start();
}

int Process::Staging::execute()
{
    return start()->wait();
}

Ref<Process::Staging> Process::stage(const String &command)
{
    return new Staging{command};
}

Ref<Process> Process::open(const String &command)
{
    return stage(command)->open();
}

Ref<Process> Process::start(const String &command)
{
    return stage(command)->start();
}

int Process::execute(const String &command)
{
    return stage(command)->execute();
}

Ref<Process> Process::bootstrap(const Staging *staging)
{
    /// locate executable and prepare argument list

    String execPath;
    Ref<const StringList> args = staging->args_;

    if (staging->command_ != "") {
        String cmd = staging->command_;
        if (cmd->startsWith(' ') || cmd->endsWith(' '))
            cmd = cmd->trim();
        if (!cmd->contains(' ')) execPath = cmd;
        if (!args) args = cmd->simplify()->split(' ');
    }

    if (args) if (args->count() == 0) args = 0;

    if (execPath == "" && args) execPath = args->at(0);

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
        int argc = args ? args->count() : 1;
        argv = new char*[argc + 1];

        if (args) {
            for (int i = 0; i < args->count(); ++i)
                argv[i] = cc::strdup(args->at(i)->chars());
        }
        else {
            argv[0] = cc::strdup(execPath->chars());
        }
        argv[argc] = 0;

        const EnvMap *envMap = staging->envMap_;
        if (envMap) {
            int n = envMap->count();
            envp = new char*[n + 1];
            for (int i = 0; i < n; ++i)
                envp[i] = cc::strcat(envMap->keyAt(i)->chars(), "=", envMap->valueAt(i)->chars());
            envp[n] = 0;
        }
    }

    /// spawn new child process

    pid_t pid = -1;
    int ret = ::posix_spawn(&pid, execPath, &staging->fileActions_, &staging->spawnAttributes_, argv, envp ? envp : ::environ);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);

    if (envp) {
        for (int i = 0; envp[i]; ++i)
            cc::free(envp[i]);
        delete[] envp;
    }

    return new Process{pid, staging};
}

Process::Process(pid_t pid, const Staging *staging):
    pid_{pid},
    groupLead_{staging->groupLead_}
{
    for (int i = 0; i <= 2; ++i)
        standardStreams_[i] = staging->standardStreams_[i];
}

Process::~Process()
{
    wait();
}

pid_t Process::id() const
{
    return pid_;
}

void Process::kill(Signal signal)
{
    if (::kill(groupLead_ ? -pid_ : pid_, +signal) == -1) {
        if (errno == EPERM) throw PermissionError{};
        CC_SYSTEM_DEBUG_ERROR(errno);
    }
}

int Process::wait()
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

SystemStream *Process::input() const
{
    return standardStreams_[0];
}

SystemStream *Process::output() const
{
    return standardStreams_[1];
}

SystemStream *Process::error() const
{
    return standardStreams_[2];
}

void Process::setWorkingDirectory(const String &path)
{
    if (::chdir(path) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

String Process::getCwd()
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

Ref<EnvMap> Process::getEnvMap()
{
    char **env = environ();
    auto map = EnvMap::create();
    for (int i = 0; env[i]; ++i) {
        String s{env[i]};
        int j = 0;
        if (s->search('=', &j)) {
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

void Process::kill(pid_t processId, Signal signal)
{
    if (::kill(processId, +signal) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::killGroup(pid_t processGroupId, Signal signal)
{
    Process::kill(-processGroupId, signal);
}

void Process::raise(Signal signal)
{
    if (::raise(+signal) == -1)
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

} // namesp
