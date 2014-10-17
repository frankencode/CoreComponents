/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_STDIO_H
#define FLUX_STDIO_H

#include <flux/File>
#include <flux/Format>
#include <flux/LineSource>

namespace flux {

File *in();
File *out();
File *err();

inline Format fout(String pattern = "") { return Format(pattern, out()); }
inline Format ferr(String pattern = "") { return Format(pattern, err()); }

LineSource *lineInput();

} // namespace flux

#endif // FLUX_STDIO_H
