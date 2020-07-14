/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/SystemSignal>
#include <cc/Format>

namespace cc {

String str(SystemSignal signal)
{
    String s;
    switch (signal) {
        case SystemSignal::Abort                 : s = "SystemSignal::Abort"; break;
        case SystemSignal::Alarm                 : s = "SystemSignal::Alarm"; break;
        case SystemSignal::Bus                   : s = "SystemSignal::Bus"; break;
        case SystemSignal::Child                 : s = "SystemSignal::Child"; break;
        case SystemSignal::Continue              : s = "SystemSignal::Continue"; break;
        case SystemSignal::FloatingPointException: s = "SystemSignal::FloatingPointException"; break;
        case SystemSignal::HangUp                : s = "SystemSignal::HangUp"; break;
        case SystemSignal::Interrupt             : s = "SystemSignal::Interrupt"; break;
        case SystemSignal::Kill                  : s = "SystemSignal::Kill"; break;
        case SystemSignal::Pipe                  : s = "SystemSignal::Pipe"; break;
        case SystemSignal::Quit                  : s = "SystemSignal::Quit"; break;
        case SystemSignal::SegmentationViolation : s = "SystemSignal::SegmentationViolation"; break;
        case SystemSignal::Stop                  : s = "SystemSignal::Stop"; break;
        case SystemSignal::Terminate             : s = "SystemSignal::Terminate"; break;
        case SystemSignal::User1                 : s = "SystemSignal::User1"; break;
        case SystemSignal::User2                 : s = "SystemSignal::User2"; break;
        case SystemSignal::WindowSizeChanged     : s = "SystemSignal::"; break;
        case SystemSignal::Undefined             : s = "SystemSignal::Undefined"; break;
        default                            : s = Format{"SystemSignal(%%)"} << +signal; break;
    };
    return s;
}

Signaled::Signaled(SystemSignal signal):
    signal_{signal}
{}

String Signaled::message() const
{
    return Format{} << "Caught signal " << signal_;
}

} // namespace cc
