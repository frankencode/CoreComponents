/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/SystemLog>
#include <cc/node/SystemLogStream>
#include <cc/ThreadLocalSingleton>
#include <cc/LineBuffer>

namespace cc {
namespace node {

void SystemLog::open(const String &identifier, int option, int facility)
{
    openlog(identifier, option, facility);
}

SystemLog *SystemLog::instance()
{
    return ThreadLocalSingleton<SystemLog>::instance();
}

SystemLog::SystemLog():
    emergencyStream_{LineBuffer::open(SystemLogStream::open(LOG_EMERG))},
    alertStream_    {LineBuffer::open(SystemLogStream::open(LOG_ALERT))},
    criticalStream_ {LineBuffer::open(SystemLogStream::open(LOG_CRIT))},
    errorStream_    {LineBuffer::open(SystemLogStream::open(LOG_ERR))},
    warningStream_  {LineBuffer::open(SystemLogStream::open(LOG_WARNING))},
    noticeStream_   {LineBuffer::open(SystemLogStream::open(LOG_NOTICE))},
    infoStream_     {LineBuffer::open(SystemLogStream::open(LOG_INFO))},
    debugStream_    {LineBuffer::open(SystemLogStream::open(LOG_DEBUG))}
{}

}} // namespace cc::node
