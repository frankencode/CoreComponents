/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/exceptions>
#include <flux/assert>

namespace flux {

void throwDebugError(const char *reason, const char *source, int line)
{
	throw DebugError(reason, source, line);
}

} // namespace flux
