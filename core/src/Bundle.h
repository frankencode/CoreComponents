/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

/** \file Bundle.h
  * \brief Lookup files in installation directories
  */

#include <cc/File>

namespace cc {

#define CC_XSTR(s) CC_STR(s)
#define CC_STR(s) #s

#define CC_BUNDLE_LOOKUP(relPath) \
    cc::File::locate(relPath, cc::StringList::create() << CC_XSTR(CCBUILD_BUNDLE_PREFIX) << cc::String(__FILE__)->reducePath(), cc::FileOk)

#define CC_BUNDLE_VERSION CC_XSTR(CCBUILD_BUNDLE_VERSION)

} // namespace cc
