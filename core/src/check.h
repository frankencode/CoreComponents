/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

/** \file check
  * \brief Check for fatal conditions
  * \see assert
  */

namespace cc {

void abort();
inline void check(bool condition) { if (!(condition)) abort(); }

} // namespace cc
