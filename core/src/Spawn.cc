/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/wait.h> // waitpid
#include <cc/strings> // cc::strdup, cc::free
#include <cc/File>
#include <cc/SocketPair>
#include <cc/Spawn>

namespace cc {

#define CC_SPAWN_CALL(call) \
{\
    int ret = call; \
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret); \
}

Spawn::Staging::Staging(const String &command):
    command_{command},
    spawnFlags_{0}
{
    CC_SPAWN_CALL(posix_spawnattr_init(&spawnAttributes_));
    CC_SPAWN_CALL(posix_spawn_file_actions_init(&fileActions_));
}

Spawn::Staging::~Staging()
{
    posix_spawn_file_actions_destroy(&fileActions_);
    posix_spawnattr_destroy(&spawnAttributes_);
}

void Spawn::Staging::enableSpawnFlag(short flag)
{
    short flags = 0;
    CC_SPAWN_CALL(posix_spawnattr_getflags(&spawnAttributes_, &flags));
    flags |= flag;
    CC_SPAWN_CALL(posix_spawnattr_setflags(&spawnAttributes_, flags));
}

Spawn::Staging *Spawn::Staging::setArgs(const StringList *args)
{
    args_ = args;
    return this;
}

Spawn::Staging *Spawn::Staging::setEnvMap(const EnvMap *envMap)
{
    envMap_ = envMap;
    return this;
}

Spawn::Staging *Spawn::Staging::setProcessGroup(pid_t groupId)
{
    CC_SPAWN_CALL(posix_spawnattr_setpgroup(&spawnAttributes_, groupId));
    enableSpawnFlag(POSIX_SPAWN_SETPGROUP);
    return this;
}

Spawn::Staging *Spawn::Staging::setSignalDefault(const SignalSet *set)
{
    CC_SPAWN_CALL(posix_spawnattr_setsigdefault(&spawnAttributes_, set->rawSet()));
    enableSpawnFlag(POSIX_SPAWN_SETSIGDEF);
    return this;
}

Spawn::Staging *Spawn::Staging::setSignalMask(const SignalSet *mask)
{
    CC_SPAWN_CALL(posix_spawnattr_setsigmask(&spawnAttributes_, mask->rawSet()));
    enableSpawnFlag(POSIX_SPAWN_SETSIGMASK);
    return this;
}

Spawn::Staging *Spawn::Staging::setInputChannel(IoChannel *channel)
{
    if (inputChannel_) return this;

    inputChannel_ = channel;

    if (channel != outputChannel_ && channel != errorChannel_)
        CC_SPAWN_CALL(posix_spawn_file_actions_addclose(&fileActions_, channel->masterFd()));

    CC_SPAWN_CALL(posix_spawn_file_actions_adddup2(&fileActions_, channel->slaveFd(), 0));

    return this;
}

Spawn::Staging *Spawn::Staging::setOutputChannel(IoChannel *channel)
{
    if (outputChannel_) return this;

    outputChannel_ = channel;

    if (channel != inputChannel_ && channel != errorChannel_)
        CC_SPAWN_CALL(posix_spawn_file_actions_addclose(&fileActions_, channel->masterFd()));

    CC_SPAWN_CALL(posix_spawn_file_actions_adddup2(&fileActions_, channel->slaveFd(), 1));

    return this;
}

Spawn::Staging *Spawn::Staging::setErrorChannel(IoChannel *channel)
{
    if (errorChannel_) return this;

    errorChannel_ = channel;

    if (channel != inputChannel_ && channel != outputChannel_)
        CC_SPAWN_CALL(posix_spawn_file_actions_addclose(&fileActions_, channel->masterFd()));

    CC_SPAWN_CALL(posix_spawn_file_actions_adddup2(&fileActions_, channel->slaveFd(), 2));
    return this;
}

Spawn::Staging *Spawn::Staging::setInputFile(const String &path)
{
    addOpenFile(0, path, OpenMode::ReadOnly);
    return this;
}

Spawn::Staging *Spawn::Staging::setOutputFile(const String &path)
{
    addOpenFile(1, path, OpenMode::WriteOnly|OpenMode::Create|OpenMode::Truncate);
    return this;
}

Spawn::Staging *Spawn::Staging::setErrorFile(const String &path)
{
    addOpenFile(2, path, OpenMode::WriteOnly|OpenMode::Create|OpenMode::Truncate);
    return this;
}

Spawn::Staging *Spawn::Staging::addOpenFile(int fd, const String &path, OpenMode openMode, FileMode fileMode)
{
    CC_SPAWN_CALL(posix_spawn_file_actions_addopen(&fileActions_, fd, path, static_cast<int>(openMode), static_cast<mode_t>(fileMode)));
    return this;
}

Ref<Spawn> Spawn::Staging::start() const
{
    return Spawn::bootstrap(this);
}

Ref<Spawn::Staging> Spawn::stage(const String &command)
{
    return new Staging{command};
}

Ref<Spawn> Spawn::bootstrap(const Staging *staging)
{
    // ------------------------------------------------------------------------
    // locate executable and prepare argument list
    // ------------------------------------------------------------------------

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
            if (!File::access(execPath, Access::Execute)) throw CommandNotFound{execPath};
        }
        else {
            String path = File::locate(execPath, Process::env("PATH")->split(':'), Access::Execute);
            if (path == "") throw CommandNotFound{execPath};
            execPath = path;
        }
    }

    // ------------------------------------------------------------------------
    // prepare argument list and environment map
    // ------------------------------------------------------------------------

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

    // ------------------------------------------------------------------------
    // spawn new child process
    // ------------------------------------------------------------------------

    pid_t pid = -1;
    int ret = ::posix_spawn(&pid, execPath, &staging->fileActions_, &staging->spawnAttributes_, argv, envp);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);

    return new Spawn{pid};
}

Spawn::Spawn(pid_t pid):
    pid_{pid}
{}

pid_t Spawn::pid() const
{
    return pid_;
}

void Spawn::kill(int signal)
{
    if (::kill(pid_, signal) == -1) {
        if (errno == EPERM)
            throw PermissionError{};
        else
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
}

void Spawn::killGroup(int signal)
{
    if (::kill(pid_, signal) == -1) {
        if (errno == EPERM)
            throw PermissionError{};
        else
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
}

int Spawn::wait(bool *signalled)
{
    int ret = 0;

    if (::waitpid(pid_, &ret, 0) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    if (WIFEXITED(ret)) {
        if (signalled) *signalled = false;
        ret = WEXITSTATUS(ret);
    }
    else if (WIFSIGNALED(ret)) {
        if (signalled) *signalled = true;
        ret = WTERMSIG(ret);
    }
    else {
        if (signalled) *signalled = false;
    }

    pid_ = -1;

    return ret;
}

} // namespace cc
