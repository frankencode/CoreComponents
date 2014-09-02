/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/ThreadLocalSingleton.h>
#include "AccessLog.h"

namespace fluxnode
{

AccessLog *accessLog() { return ThreadLocalSingleton<AccessLog>::instance(); }

} // namespace fluxnode
