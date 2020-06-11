/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/InputPipe>
#include <cc/OutputPipe>
#include <cc/Command>

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
    while (channelHead_) {
        channelHead_->onStart();
        Ref<IoChannel> next = channelHead_->next_;
        channelHead_->next_ = nullptr;
        channelHead_ = next;
    }
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

Command::Instance *Command::Instance::setSignalDefault(const SignalSet *set)
{
    CC_SPAWN_CALL(posix_spawnattr_setsigdefault(&spawnAttributes_, set->rawSet()));
    enableSpawnFlag(POSIX_SPAWN_SETSIGDEF);
    return this;
}

Command::Instance *Command::Instance::setSignalMask(const SignalSet *mask)
{
    CC_SPAWN_CALL(posix_spawnattr_setsigmask(&spawnAttributes_, mask->rawSet()));
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

Command::Instance *Command::Instance::setInputChannel(IoChannel *channel)
{
    return attachChannel(channel, 0);
}

Command::Instance *Command::Instance::setOutputChannel(IoChannel *channel)
{
    return attachChannel(channel, 1);
}

Command::Instance *Command::Instance::setErrorChannel(IoChannel *channel)
{
    return attachChannel(channel, 2);
}

Command::Instance *Command::Instance::attachChannel(IoChannel *channel, int targetFd)
{
    if (!channel->next_) {
        channel->next_ = channelHead_;
        channelHead_ = channel;
    }
    if (0 <= targetFd && targetFd <= 2) standardStreams_[targetFd] = channel;
    CC_SPAWN_CALL(posix_spawn_file_actions_adddup2(&fileActions_, channel->slaveFd(), targetFd));
    return this;
}

Command::Instance *Command::Instance::setInput(SystemStream *stream)
{
    return attach(stream, 0);
}

Command::Instance *Command::Instance::setOutput(SystemStream *stream)
{
    return attach(stream, 1);
}

Command::Instance *Command::Instance::setError(SystemStream *stream)
{
    return attach(stream, 2);
}

Command::Instance *Command::Instance::attach(SystemStream *stream, int targetFd)
{
    if (0 <= targetFd && targetFd <= 2) standardStreams_[targetFd] = stream;
    CC_SPAWN_CALL(posix_spawn_file_actions_adddup2(&fileActions_, stream->fd(), targetFd));
    return this;
}

Command::Instance *Command::Instance::simpleRedirection()
{
    if (!standardStreams_[0]) setInputChannel(InputPipe::create());

    if (!standardStreams_[1] || !standardStreams_[2]) {
        auto channel = OutputPipe::create();
        if (!standardStreams_[1]) setOutputChannel(channel);
        if (!standardStreams_[2]) setErrorChannel(channel);
    }

    return this;
}

} // namespace cc
