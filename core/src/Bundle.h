/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_BUNDLE_H
#define FLUX_BUNDLE_H

/** \file Bundle.h
  * \brief Lookup files in installation directories
  */

#include <flux/File>

namespace flux {

#define FLUX_XSTR(s) FLUX_STR(s)
#define FLUX_STR(s) #s

#define FLUX_BUNDLE_LOOKUP(relPath) \
    flux::File::lookup(relPath, flux::StringList::create() << FLUX_XSTR(FLUXMAKE_BUNDLE_PREFIX) << flux::String(__FILE__)->reducePath(), flux::File::Exists)

#define FLUX_BUNDLE_VERSION FLUX_XSTR(FLUXMAKE_BUNDLE_VERSION)

} // namespace flux

#endif // FLUX_BUNDLE_H
