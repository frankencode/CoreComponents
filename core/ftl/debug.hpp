/*
 * debug.hpp -- low-level debugging helper
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_DEBUG_HPP
#define FTL_DEBUG_HPP

namespace ftl
{

void abort();
inline void check(bool condition) { if (!(condition)) abort(); }

} // namespace ftl

#endif // FTL_DEBUG_HPP
