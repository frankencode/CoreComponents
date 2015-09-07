/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_STDIO_H
#define FLUX_STDIO_H

/** \file stdio
  * \brief Standard input/output streams
  */

#include <flux/File>
#include <flux/Format>
#include <flux/LineSource>

namespace flux {

File *stdIn();
File *stdOut();
File *stdErr();

inline Format fout(String pattern = "") { return Format(pattern, stdOut()); }
inline Format ferr(String pattern = "") { return Format(pattern, stdErr()); }

LineSource *lineInput();

} // namespace flux

#endif // FLUX_STDIO_H
