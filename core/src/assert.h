/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_ASSERT_H
#define FLUX_ASSERT_H

/** \file assert
  *  \brief Assertion macros (debug mode)
  */

namespace flux {

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
