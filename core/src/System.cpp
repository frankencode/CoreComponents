/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <unistd.h> // sysconf(3), alarm(2)
#ifndef __linux
#include <sys/types.h>
#ifdef __OpenBSD__
#include <sys/param.h>
#endif
#include <sys/sysctl.h>
#endif
#include <sys/time.h>
#include <time.h>
#include <flux/exceptions>
#include <flux/System>

namespace flux {

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
    struct timeval tv;
    ::gettimeofday(&tv, 0);
    return double(tv.tv_sec) + double(tv.tv_usec) / 1e6;
}

/** Return the name of this host
  */
String System::hostName()
{
    const int bufSize = 1024;
    char buf[bufSize + 1];
    String name;
    if (::gethostname(buf, bufSize) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
    buf[bufSize] = 0;
    name = buf;
    return name;
}

/** Set the name of this host
  */
void System::setHostName(const String &newName)
{
    if (::sethostname(newName->chars(), newName->count()) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
}

} // namespace flux
