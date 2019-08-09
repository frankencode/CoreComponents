/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "Log.h"

namespace cc { template<class> class ThreadLocalSingleton; }

namespace ccnode {

class AccessLog: public Log
{
public:
    static AccessLog *instance();

private:
    friend class ThreadLocalSingleton<AccessLog>;
};

} // namespace ccnode
