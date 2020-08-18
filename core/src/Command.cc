/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Command>
#include <cc/SocketPair>

namespace cc {

#define CC_SPAWN_CALL(call) \
{\
    int ret = call; \
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret); \
}

Command::Instance::Instance(const String &command):
    command_{command}
{
    CC_SPAWN_CALL(posix_spawnattr_init(&spawnAttributes_));
    CC_SPAWN_CALL(posix_spawn_file_actions_init(&fileActions_));
}

Command::Instance::~Instance()
{
    posix_spawn_file_actions_destroy(&fileActions_);
    posix_spawnattr_destroy(&spawnAttributes_);
}

void Command::Instance::startDone()
{
    for (IoChannel &channel: ioChannels_)
        channel->onStart();

    ioChannels_->deplete();
}

void Command::Instance::enableSpawnFlag(short flag)
{
    short flags = 0;
    CC_SPAWN_CALL(posix_spawnattr_getflags(&spawnAttributes_, &flags));
    flags |= flag;
    CC_SPAWN_CALL(posix_spawnattr_setflags(&spawnAttributes_, flags));
}

Command::Instance *Command::Instance::setArgs(const StringList &args)
{
    args_ = args;
    return this;
}

Command::Instance *Command::Instance::setEnvMap(const EnvMap &envMap)
{
    customEnvMap_ = true;
    envMap_ = envMap;
    return this;
}

Command::Instance *Command::Instance::setProcessGroup(pid_t groupId)
{
    CC_SPAWN_CALL(posix_spawnattr_setpgroup(&spawnAttributes_, groupId));
    enableSpawnFlag(POSIX_SPAWN_SETPGROUP);
    groupLead_ = (groupId == 0);
    return this;
}

Command::Instance *Command::Instance::setSignalDefault(const SignalSet &set)
{
    CC_SPAWN_CALL(posix_spawnattr_setsigdefault(&spawnAttributes_, set));
    enableSpawnFlag(POSIX_SPAWN_SETSIGDEF);
    return this;
}

Command::Instance *Command::Instance::setSignalMask(const SignalSet &mask)
{
    CC_SPAWN_CALL(posix_spawnattr_setsigmask(&spawnAttributes_, mask));
    enableSpawnFlag(POSIX_SPAWN_SETSIGMASK);
    return this;
}

Command::Instance *Command::Instance::setWorkingDirectory(const String &path)
{
    #ifdef __GLIBC__
    #if __GLIBC_PREREQ(2, 29)
    CC_SPAWN_CALL(posix_spawn_file_actions_addchdir_np(&fileActions_, path));
    #else
    cwd_ = path;
    #endif
    #else
    CC_SPAWN_CALL(posix_spawn_file_actions_addchdir_np(&fileActions_, path));
    #endif
    return this;
}

Command::Instance *Command::Instance::setInputChannel(const IoChannel &channel)
{
    return attachChannel(channel, 0);
}

Command::Instance *Command::Instance::setOutputChannel(const IoChannel &channel)
{
    return attachChannel(channel, 1);
}

Command::Instance *Command::Instance::setErrorChannel(const IoChannel &channel)
{
    return attachChannel(channel, 2);
}

Command::Instance *Command::Instance::attachChannel(const IoChannel &channel, int targetFd)
{
    ioChannels_->append(channel);
    if (0 <= targetFd && targetFd <= 2) standardStreams_[targetFd] = channel;
    CC_SPAWN_CALL(posix_spawn_file_actions_adddup2(&fileActions_, channel->slaveFd(), targetFd));
    return this;
}

Command::Instance *Command::Instance::setInput(const SystemStream &stream)
{
    return attach(stream, 0);
}

Command::Instance *Command::Instance::setOutput(const SystemStream &stream)
{
    return attach(stream, 1);
}

Command::Instance *Command::Instance::setError(const SystemStream &stream)
{
    return attach(stream, 2);
}

Command::Instance *Command::Instance::attach(const SystemStream &stream, int targetFd)
{
    if (0 <= targetFd && targetFd <= 2) standardStreams_[targetFd] = stream;
    CC_SPAWN_CALL(posix_spawn_file_actions_adddup2(&fileActions_, stream->fd(), targetFd));
    return this;
}

Command::Instance *Command::Instance::simpleRedirection()
{
    if (!standardStreams_[0]) setInputChannel(SocketPair{});

    if (!standardStreams_[1] || !standardStreams_[2]) {
        SocketPair outputPipe;
        if (!standardStreams_[1]) setOutputChannel(outputPipe);
        if (!standardStreams_[2]) setErrorChannel(outputPipe);
    }

    return this;
}

} // namespace cc
