/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/exceptions>
#include <cc/str>

namespace cc {

String OutputExhaustion::message() const
{
    return "Output exhaustion";
}

String InputExhaustion::message() const
{
    return "Input exhaustion";
}

String Timeout::message() const
{
    return "Operation timeout";
}

String HelpRequest::message() const
{
    return "No help, yet ...";
}

String VersionRequest::message() const
{
    return "Application is not versioned, yet ...";
}

String PropertyBindingLoop::message() const
{
    return "Property binding loop detected";
}

String CommandNotFound::message() const
{
    return List<String>{} << "Command not found: '" << command_ << "'";
}

String DebugError::message() const
{
    return List<String>{} << source_ << ":" << str(line_) << ": " << reason_;
}

} // namespace cc
