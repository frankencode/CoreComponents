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

#include <flux/SystemStream>
#include <flux/Format>

namespace flux {

SystemStream *stdIn();
SystemStream *stdOut();
SystemStream *stdErr();

inline Format fout(String pattern = "") { return Format(pattern, stdOut()); }
inline Format ferr(String pattern = "") { return Format(pattern, stdErr()); }

} // namespace flux

#endif // FLUX_STDIO_H
