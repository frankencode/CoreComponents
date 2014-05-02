/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_ASSERT_H
#define FLUX_ASSERT_H

namespace flux
{

void throwDebugError(const char *reason, const char *source, int line);

#ifndef NDEBUG
#define FLUX_ASSERT(condition) \
	if (!(condition)) throwDebugError(#condition, __FILE__, __LINE__);
#else
#define FLUX_ASSERT(condition) ;
#endif

#ifndef NDEBUG
#define FLUX_ASSERT2(condition, reason) \
	if (!(condition)) throwDebugError(reason, __FILE__, __LINE__);
#else
#define FLUX_ASSERT2(condition, reason);
#endif

} // namespace flux

#endif // FLUX_ASSERT_H
