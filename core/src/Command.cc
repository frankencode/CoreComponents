/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Command>
#include <cc/LocalChannel>
#include <cc/Process>
#include <cc/File>
#include <cstring>
#include <spawn.h>

namespace cc {

#define CC_SPAWN_CALL(call) \
{\
    int ret = call; \
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret); \
}

struct Command::State: public Object::State
{
    State(const String &command = String{}):
        command_{command}
    {
        CC_SPAWN_CALL(posix_spawnattr_init(&spawnAttributes_));
        CC_SPAWN_CALL(posix_spawn_file_actions_init(&fileActions_));
    }

    ~State()
    {
        posix_spawn_file_actions_destroy(&fileActions_);
        posix_spawnattr_destroy(&spawnAttributes_);
    }

    void enableSpawnFlag(short flag)
    {
        short flags = 0;
        CC_SPAWN_CALL(posix_spawnattr_getflags(&spawnAttributes_, &flags));
        flags |= flag;
        CC_SPAWN_CALL(posix_spawnattr_setflags(&spawnAttributes_, flags));
    }

    String command_;
    String execPath_;
    List<String> args_;
    Map<String> envMap_;
    bool inheritEnvMap_ { true };

    int spawnFlags_ { 0 };
    posix_spawnattr_t spawnAttributes_;
    posix_spawn_file_actions_t fileActions_;
    bool groupLead_ { false };
    List<IoChannel> ioChannels_;
    List<IoStream> ioStreams_;
    IoStream standardStreams_[3];

    #ifdef __GLIBC__
    #if __GLIBC_PREREQ(2, 29)
    #else
    String cwd_;
    #endif
    #endif
};

Command::Command():
    Object{new State}
{}

Command::Command(const String &command):
    Object{new State{command}}
{}

Command &Command::args(const List<String> &args)
{
    me().args_ = args;
    return *this;
}

Command &Command::env(const Map<String> &map)
{
    me().envMap_ = map;
    me().inheritEnvMap_ = false;
    return *this;
}

Command &Command::group(int groupId)
{
    CC_SPAWN_CALL(posix_spawnattr_setpgroup(&me().spawnAttributes_, groupId));
    me().enableSpawnFlag(POSIX_SPAWN_SETPGROUP);
    me().groupLead_ = (groupId == 0);
    return *this;
}

Command &Command::signalMask(const SignalSet &mask)
{
    CC_SPAWN_CALL(posix_spawnattr_setsigmask(&me().spawnAttributes_, mask));
    me().enableSpawnFlag(POSIX_SPAWN_SETSIGMASK);
    return *this;
}

Command &Command::signalDefault(const SignalSet &set)
{
    CC_SPAWN_CALL(posix_spawnattr_setsigdefault(&me().spawnAttributes_, set));
    me().enableSpawnFlag(POSIX_SPAWN_SETSIGDEF);
    return *this;
}

Command &Command::workDir(const String &path)
{
    #ifdef __GLIBC__
    #if __GLIBC_PREREQ(2, 29)
    CC_SPAWN_CALL(posix_spawn_file_actions_addchdir_np(&me().fileActions_, path));
    #else
    cwd_ = path;
    #endif
    #else
    CC_SPAWN_CALL(posix_spawn_file_actions_addchdir_np(&me().fileActions_, path));
    #endif
    return *this;
}

Command &Command::io(int fd, const IoChannel &channel)
{
    me().ioChannels_.append(channel);
    if (0 <= fd && fd <= 2) me().standardStreams_[fd] = channel;
    CC_SPAWN_CALL(posix_spawn_file_actions_adddup2(&me().fileActions_, channel.slaveFd(), fd));
    return *this;
}

Command &Command::io(int fd, const IoStream &stream)
{
    if (0 <= fd && fd <= 2) me().standardStreams_[fd] = stream;
    else me().ioStreams_.append(stream);
    if (stream.fd() != fd)
        CC_SPAWN_CALL(posix_spawn_file_actions_adddup2(&me().fileActions_, stream.fd(), fd));
    return *this;
}

void Command::start(Process &process)
{
    /// automatically setup I/O redirection if not requested otherwise

    if (
        !me().standardStreams_[0] ||
        !me().standardStreams_[1] ||
        !me().standardStreams_[2]
    ) {
        LocalChannel channel;
        if (!me().standardStreams_[0]) io(0, channel);
        if (!me().standardStreams_[1]) io(1, channel);
        if (!me().standardStreams_[2]) io(2, channel);
    }

    /// locate executable and prepare argument list

    String execPath;
    List<String> args = me().args_;

    if (me().command_ != "") {
        String cmd = me().command_.copy();
        if (cmd.startsWith(' ') || cmd.endsWith(' '))
            cmd.trim();
        if (!cmd.find(' ')) execPath = cmd;
        if (args.count() == 0) {
            cmd.simplify();
            args = cmd.split(' ');
        }
    }

    if (execPath == "" && args.count() > 0) execPath = args.at(0);

    if (execPath != "") {
        if (execPath.find('/')) {
            if (!File::access(execPath, FileAccess::Execute)) throw CommandNotFound{execPath};
        }
        else {
            String path = File::locate(execPath, Process::env("PATH").split(':'), FileAccess::Execute);
            if (path == "") throw CommandNotFound{execPath};
            execPath = path;
        }
    }

    /// prepare argument list and environment map

    const int argc = (args.count() > 0) ? args.count() : 1;
    char **argv = new char*[argc + 1];
    {
        if (args.count() > 0) {
            for (int i = 0; i < args.count(); ++i)
                argv[i] = String{args.at(i)};
        }
        else {
            argv[0] = execPath;
        }
        argv[argc] = 0;
    }

    char **envp = nullptr;
    if (!me().inheritEnvMap_) {
        const Map<String> &envMap = me().envMap_;
        const int n = envMap.count();
        envp = new char*[n + 1];
        int i = 0;
        for (auto &pair: envMap) {
            int n = pair.key().count() + pair.value().count() + 2;
            char *s = static_cast<char *>(::malloc(n));
            std::memcpy(s, pair.key().chars(), pair.key().count());
            s[pair.key().count()] = '=';
            std::memcpy(s + pair.key().count() + 1, pair.value().chars(), pair.value().count());
            s[n - 1] = '\0';
            envp[i] = s;
            ++i;
        }
        envp[n] = nullptr;
    }

    /// spawn new child process

    #ifdef __GLIBC__
    #if __GLIBC_PREREQ(2, 29)
    #else
    class CwdGuard {
    public:
        CwdGuard(const String &cwd):
            cwdSaved_{cwd != "" ? Process::cwd() : String{}}
        {
            if (cwd != "") Process::setCwd(cwd);
        }
        ~CwdGuard()
        {
            if (cwdSaved_ != "") Process::setCwd(cwdSaved_);
        }
    private:
        String cwdSaved_;
    };
    CwdGuard guard{me().cwd_};
    #endif
    #endif

    process.me().pid_ = -1;
    int ret = ::posix_spawn(&process.me().pid_, execPath, &me().fileActions_, &me().spawnAttributes_, argv, envp ? envp : ::environ);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);

    delete[] argv;
    if (envp) {
        for (int i = 0; envp[i]; ++i)
            ::free(envp[i]);
        delete[] envp;
    }

    process.me().groupLead_ = me().groupLead_;

    for (int i = 0; i <= 2; ++i)
        process.me().standardStreams_[i] = me().standardStreams_[i];

    for (IoChannel &channel: me().ioChannels_)
        channel.me().onStart();
}

Command::State &Command::me()
{
    return Object::me.as<State>();
}

const Command::State &Command::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
