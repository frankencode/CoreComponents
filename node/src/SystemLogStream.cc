/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/SystemLogStream>
#include <syslog.h>

namespace cc {
namespace http {

SystemLogStream::Instance::Instance(int priority):
    priority_{priority}
{}

void SystemLogStream::Instance::write(const CharArray *data)
{
    syslog(priority_, "%s", data->chars());
}

void SystemLogStream::Instance::write(const StringList &parts)
{
    write(parts->join());
}

}} // namespace cc::http
