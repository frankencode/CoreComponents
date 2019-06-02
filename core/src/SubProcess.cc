/*
 * Copyright (C) 2007-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/types.h> // pid_t, mode_t, uid_t, gid_t
#include <sys/ioctl.h> // ioctl
#include <sys/stat.h> // umask
#include <sys/socket.h> // socketpair
#include <sys/wait.h> // waitpid
#include <unistd.h> // fork, execve, dup2, chdir, close, environ
#include <fcntl.h> // open
#include <stdlib.h> // exit, posix_openpt, grantpt, unlockpt
#include <termios.h> // tcgetattr, tcsetattr
#include <cc/strings> // cc::strdup, cc::free
#include <cc/File>
#include <cc/Process>
#include <cc/SubProcess>

namespace cc {

Ref<SubProcess> SubProcess::bootstrap(Staging *staging)
{
    // ------------------------------------------------------------------------
    // locate executable and prepare argument list
    // ------------------------------------------------------------------------

    String execPath;
    Ref<StringList> args = staging->args_;

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
            if (staging->workDir_ != "" && execPath->isRelativePath()) execPath = execPath->absolutePathRelativeTo(Process::cwd());
            if (!File::access(execPath, Access::Execute)) throw CommandNotFound{execPath};
        }
        else {
            String path = File::locate(execPath, Process::env("PATH")->split(':'), Access::Execute);
            if (path == "") throw CommandNotFound{execPath};
            execPath = path;
        }
    }

    // ------------------------------------------------------------------------
    // prepare I/O forwarding
    // ------------------------------------------------------------------------

    enum { Master = 0, Slave = 1 };

    int fd[2];
    fd[Master] = 0;
    fd[Slave] = 0;

    if (staging->forwarding_ == SubProcess::ForwardBySocket)
    {
        if (::socketpair(AF_LOCAL, SOCK_STREAM, 0, fd) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
    else if (staging->forwarding_ == SubProcess::ForwardByPts)
    {
        fd[Master] = ::posix_openpt(O_RDWR|O_NOCTTY);
        if (fd[Master] == -1) CC_SYSTEM_DEBUG_ERROR(errno);
        if (::grantpt(fd[Master]) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
        if (::unlockpt(fd[Master]) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
        fd[Slave] = ::open(ptsname(fd[Master]), O_RDWR|O_NOCTTY);
        if (fd[Slave] == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    }

    // ------------------------------------------------------------------------
    // prepare argument list and environment map
    // ------------------------------------------------------------------------

    char **argv = nullptr;
    char **envp = nullptr;

    if (execPath != "")
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

        EnvMap *envMap = staging->envMap_;
        if (envMap) {
            int n = envMap->count();
            envp = new char*[n + 1];
            for (int i = 0; i < n; ++i)
                envp[i] = cc::strcat(envMap->keyAt(i)->chars(), "=", envMap->valueAt(i)->chars());
            envp[n] = 0;
        }
    }

    // ------------------------------------------------------------------------
    // fork and setup child process
    // ------------------------------------------------------------------------

    int ret = ::fork();
    if (ret < 0) CC_SYSTEM_DEBUG_ERROR(errno);

    if (ret == 0)
    {
        if (staging->type_ == SubProcess::GroupLeader)
            ::setpgid(0, 0);
        else if (staging->type_ == SubProcess::SessionLeader)
            ::setsid();

        if (staging->forwarding_ == SubProcess::ForwardBySocket)
        {
            ::close(fd[Master]);
            ::dup2(fd[Slave], 0);
            ::dup2(fd[Slave], 1);
            ::dup2(fd[Slave], 2);
        }
        else if (staging->forwarding_ == SubProcess::ForwardByPts)
        {
            ::close(fd[Master]);
            ::dup2(fd[Slave], 0);
            ::dup2(fd[Slave], 1);
            ::dup2(fd[Slave], 2);

            if (staging->type_ == SubProcess::SessionLeader)
                ::ioctl(fd[Slave], TIOCSCTTY, 0);

            {
                struct termios attr;
                ::tcgetattr(fd[Slave], &attr);
                attr.c_iflag ^= IXON;
            #ifdef IUTF8
                attr.c_iflag |= IUTF8;
            #endif // IUTF8
                ::tcsetattr(fd[Slave], TCSANOW, &attr);
            }
        }

        if (staging->overloads_) {
            Overloads *overloads = staging->overloads_;
            for (int i = 0; i < overloads->count(); ++i)
                ::dup2(overloads->valueAt(i)->fd(), overloads->keyAt(i));
        }

        if (staging->workDir_ != "") {
            int ret = ::chdir(staging->workDir_);
            if (ret != 0) ::exit(errno);
        }

        if (staging->userMask_ >= 0)
            ::umask(staging->userMask_);

        if (staging->signalMask_)
            ::sigprocmask(SIG_SETMASK, staging->signalMask_->rawSet(), 0);
        else
            ::sigprocmask(SIG_SETMASK, SignalSet::createEmpty()->rawSet(), 0);

        if (execPath != "") {
            ::execve(execPath, argv, envp ? envp : ::environ);
            ::exit(errno);
        }

        ::exit(staging->worker_ ? staging->worker_->run() : 0);
    }

    // ------------------------------------------------------------------------
    // connect parent to child process
    // ------------------------------------------------------------------------

    if (argv) {
        for (int i = 0; argv[i]; ++i)
            cc::free(argv[i]);
        delete[] argv;
    }
    if (envp) {
        for (int i = 0; envp[i]; ++i)
            cc::free(envp[i]);
        delete[] envp;
    }

    Ref<SubProcess> child = new SubProcess(staging, ret);

    if (
        staging->forwarding_ == SubProcess::ForwardBySocket ||
        staging->forwarding_ == SubProcess::ForwardByPts
    ) {
        ::close(fd[Slave]);
        child->fd_ = fd[Master];
    }

    return child;
}

Ref<SubProcess> SubProcess::open(const String &command, SystemStream *stdErr)
{
    auto stage = SubProcess::stage();
    stage->setCommand(command);
    if (stdErr) {
        auto overloads = Overloads::create();
        overloads->establish(StandardErrorFd, stdErr);
        stage->setOverloads(overloads);
    }
    return stage->open();
}

Ref<SubProcess> SubProcess::open(SubProcess::Worker *worker, SystemStream *stdErr)
{
    auto stage = SubProcess::stage();
    stage->setWorker(worker);
    if (stdErr) {
        auto overloads = Overloads::create();
        overloads->establish(StandardErrorFd, stdErr);
        stage->setOverloads(overloads);
    }
    return stage->open();
}

Ref<SubProcess> SubProcess::start(const String &command)
{
    return stage()
        ->setCommand(command)
        ->setForwarding(NoForwarding)
        ->open();
}

int SubProcess::shellExecute(const String &shellCommand)
{
    return SubProcess::stage()
        ->setArgs(
            StringList::create()
                << Process::env("SHELL")
                << "-c"
                << shellCommand
        )
        ->start()
        ->wait();
}

SubProcess::SubProcess(Staging *staging, pid_t pid):
    params_{staging},
    pid_{pid}
{}

SubProcess::~SubProcess()
{
    if (pid_ > 0) wait();
}

pid_t SubProcess::pid() const { return pid_; }

void SubProcess::kill(int signal)
{
    if (::kill(pid_, signal) == -1) {
        if (errno == EPERM)
            throw PermissionError{};
        else
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
}

void SubProcess::killGroup(int signal)
{
    if (::kill(pid_, signal) == -1) {
        if (errno == EPERM)
            throw PermissionError{};
        else
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
}

int SubProcess::wait(bool *signalled)
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
