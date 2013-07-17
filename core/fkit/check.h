/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_CHECK_H
#define FKIT_CHECK_H

namespace fkit
{

void abort();
inline void check(bool condition) { if (!(condition)) abort(); }

} // namespace fkit

#endif // FKIT_CHECK_H
