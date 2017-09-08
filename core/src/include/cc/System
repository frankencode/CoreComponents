/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>

namespace cc {

/** \class System System.h cc/System
  * \ingroup sysinfo
  * \brief %System information
  * \see KernelInfo
  */
class System
{
public:
    static int concurrency(); ///< number of threads of the system
    static int pageSize(); ///< memory page size
    static double now(); ///< current system time

    static String hostName(); ///< system host name
    static void setHostName(String newName); ///< set system host name
};

} // namespace cc
