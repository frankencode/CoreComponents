 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_DEBUG_HPP
#define FTL_DEBUG_HPP

namespace ftl
{

void abort();
inline void check(bool condition) { if (!(condition)) abort(); }

} // namespace ftl

#endif // FTL_DEBUG_HPP
