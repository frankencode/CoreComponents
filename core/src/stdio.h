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

SystemStream *stdIn();
SystemStream *stdOut();
SystemStream *stdErr();

Format fout(String pattern = "");
Format ferr(String pattern = "");

} // namespace cc
