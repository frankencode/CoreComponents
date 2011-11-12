/*
 * debug.hpp -- low-level debugging helper
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_DEBUG_HPP
#define FTL_DEBUG_HPP

namespace ftl
{

void abort();
inline void check(bool condition) { if (!(condition)) abort(); }

} // namespace ftl

#endif // FTL_DEBUG_HPP
