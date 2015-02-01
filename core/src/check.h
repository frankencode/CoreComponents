/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_CHECK_H
#define FLUX_CHECK_H

/** \file check
  * \brief Check for fatal conditions
  * \see assert
  */

namespace flux {

void abort();
inline void check(bool condition) { if (!(condition)) abort(); }

} // namespace flux

#endif // FLUX_CHECK_H
