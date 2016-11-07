/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>

namespace cc {

/** \class System System.h cc/System
  * \brief %System information
  * \see KernelInfo
  */
class System
{
public:
    static int concurrency();
    static int pageSize();
    static double now();

    static String hostName();
    static void setHostName(String newName);
};

} // namespace cc
