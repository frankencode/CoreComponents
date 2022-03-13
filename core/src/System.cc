/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/System>
#include <unistd.h> // sysconf(3)
#ifndef __linux
#include <sys/types.h>
#ifdef __OpenBSD__
#include <sys/param.h>
#endif
#include <sys/sysctl.h>
#endif
#include <time.h>

namespace cc {

int System::concurrency()
{
    #ifdef _SC_NPROCESSORS_ONLN
        return sysconf(_SC_NPROCESSORS_ONLN);
    #endif

    #ifdef _SC_NPROC_ONLN
        return sysconf(_SC_NPROC_ONLN);
    #endif

    #ifndef __linux
    #ifdef HW_NCPU
    int n = 1;
    size_t sz = sizeof(n);
    int mib[4];
    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    sysctl(mib, 2, &n, &sz, NULL, 0);
    if(n > 1) return n;
    #endif
    #endif

    return 1;
}

int System::pageSize()
{
    return sysconf(_SC_PAGE_SIZE);
}

double System::now()
{
    struct timespec ts;
    ::clock_gettime(CLOCK_REALTIME, &ts);
    return double(ts.tv_sec) + double(ts.tv_nsec) / 1e9;
}

String System::hostName()
{
    const int bufSize = 256;
    char buf[bufSize + 1];
    if (::gethostname(buf, bufSize) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    buf[bufSize] = 0;
    return buf;
}

void System::setHostName(const String &newName)
{
    if (::sethostname(newName, newName.count()) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

} // namespace cc
