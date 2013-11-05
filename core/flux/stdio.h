/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_STDIO_H
#define FLUX_STDIO_H

#include "File.h"
#include "Format.h"
#include "LineSource.h"

namespace flux
{

File *in();
File *out();
File *err();

inline Format fout(String pattern = "") { return Format(pattern, out()); }
inline Format ferr(String pattern = "") { return Format(pattern, err()); }

LineSource *lineInput();

} // namespace flux

#endif // FLUX_STDIO_H
