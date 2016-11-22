/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

/** \file stdio
  * \brief Standard input/output streams
  */

#include <cc/SystemStream>
#include <cc/Format>

namespace cc {

/** \addtogroup binary_io
  * \{
  */

SystemStream *stdIn(); ///< Standard input stream
SystemStream *stdOut(); ///< Standard output stream
SystemStream *stdErr(); ///< Standard error stream

/** \}
  */

/** \addtogroup human_io
  * \{
  */

/// Helper method to format a standard output text
Format fout(String pattern = "");

/// Helper method to format a standard error text
Format ferr(String pattern = "");

/** \}
  */

} // namespace cc
