/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <signal.h>
#include <cc/check>

namespace cc {

void abort()
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGABRT);
    pthread_sigmask(SIG_UNBLOCK, &set, 0);
    raise(SIGABRT);
}

} // namespace cc
