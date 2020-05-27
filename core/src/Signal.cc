/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/format>
#include <cc/Signal>

namespace cc {

string str(Signal signal)
{
    string s;
    switch (signal) {
        case Signal::Abort                 : s = "Signal::Abort"; break;
        case Signal::Alarm                 : s = "Signal::Alarm"; break;
        case Signal::Bus                   : s = "Signal::Bus"; break;
        case Signal::Child                 : s = "Signal::Child"; break;
        case Signal::Continue              : s = "Signal::Continue"; break;
        case Signal::FloatingPointException: s = "Signal::FloatingPointException"; break;
        case Signal::HangUp                : s = "Signal::HangUp"; break;
        case Signal::Interrupt             : s = "Signal::Interrupt"; break;
        case Signal::Kill                  : s = "Signal::Kill"; break;
        case Signal::Pipe                  : s = "Signal::Pipe"; break;
        case Signal::Quit                  : s = "Signal::Quit"; break;
        case Signal::SegmentationViolation : s = "Signal::SegmentationViolation"; break;
        case Signal::Stop                  : s = "Signal::Stop"; break;
        case Signal::Terminate             : s = "Signal::Terminate"; break;
        case Signal::User1                 : s = "Signal::User1"; break;
        case Signal::User2                 : s = "Signal::User2"; break;
        case Signal::WindowSizeChanged     : s = "Signal::"; break;
        case Signal::Undefined             : s = "Signal::Undefined"; break;
        default                            : s = format{"Signal(%%)"} << +signal; break;
    };
    return s;
}

Signaled::Signaled(Signal signal):
    signal_{signal}
{}

string Signaled::message() const
{
    return format{} << "Caught signal " << signal_;
}

} // namespace cc
