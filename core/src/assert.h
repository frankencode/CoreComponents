/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

/** \file assert
  * \brief Assertion macros (debug mode)
  */

namespace cc {

void throwDebugError(const char *reason, const char *source, int line);

#ifndef NDEBUG
#define CC_ASSERT(condition) \
    if (!(condition)) throwDebugError(#condition, __FILE__, __LINE__);
#else
#define CC_ASSERT(condition) ;
#endif

#ifndef NDEBUG
#define CC_ASSERT2(condition, reason) \
    if (!(condition)) throwDebugError(reason, __FILE__, __LINE__);
#else
#define CC_ASSERT2(condition, reason);
#endif

template<bool b>
struct StaticAssert {};

template<>
struct StaticAssert<true> {
    static void yes() {}
};

#define CC_STATIC_ASSERT(condition) StaticAssert<condition>::yes();

} // namespace cc
