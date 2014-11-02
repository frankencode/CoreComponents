/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_RESOURCEGUARD_H
#define FLUX_RESOURCEGUARD_H

#include <flux/String>

namespace flux {

/** \brief Resource context guard
  * \see ResourceContext
  */
class ResourceGuard
{
public:
    ResourceGuard(String resource);
    ~ResourceGuard();
};

} // namespace flux

#endif // FLUX_RESOURCEGUARD_H
