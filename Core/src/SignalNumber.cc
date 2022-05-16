/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Signal>
#include <cc/Format>

namespace cc {

String str(Signal signal)
{
    String s;
    switch (signal) {
        case Signal::Abort     : s = "SIGABRT"; break;
        case Signal::Alarm     : s = "SIGALRM"; break;
        case Signal::Bus       : s = "SIGBUS"; break;
        case Signal::Child     : s = "SIGCHLD"; break;
        case Signal::Continue  : s = "SIGCONT"; break;
        case Signal::FPE       : s = "SIGFPE"; break;
        case Signal::HangUp    : s = "SIGHUP"; break;
        case Signal::Interrupt : s = "SIGINT"; break;
        case Signal::Kill      : s = "SIGKILL"; break;
        case Signal::Pipe      : s = "SIGPIPE"; break;
        case Signal::Quit      : s = "SIGQUIT"; break;
        case Signal::SegV      : s = "SIGSEGV"; break;
        case Signal::Stop      : s = "SIGSTOP"; break;
        case Signal::Terminate : s = "SIGTERM"; break;
        case Signal::User1     : s = "SIGUSR1"; break;
        case Signal::User2     : s = "SIGUSR2"; break;
        case Signal::WindowSize: s = "SIGWINCH"; break;
        case Signal::Undefined : s = "SIGUNDEF"; break;
        default                      : s = Format{"SIG%%"} << +signal; break;
    };
    return s;
}

Signaled::Signaled(Signal signal):
    signal_{signal}
{}

String Signaled::message() const
{
    return Format{} << "Caught " << signal_;
}

} // namespace cc
