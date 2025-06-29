/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/System>
#include <cc/Random>
#include <unistd.h> // sysconf(3)
#ifdef __OpenBSD__
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#endif
#include <time.h>
#include <cstdint>

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

uint32_t System::noise()
{
    static struct timespec ts;
    ::clock_gettime(CLOCK_MONOTONIC, &ts);
    return Random{static_cast<uint32_t>(ts.tv_nsec)}.get();
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
