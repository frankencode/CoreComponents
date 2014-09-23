/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "exceptions.h"
#include "assert.h"

namespace flux
{

void throwDebugError(const char *reason, const char *source, int line)
{
	throw DebugError(reason, source, line);
}

} // namespace flux
