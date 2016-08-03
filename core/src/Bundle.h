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

#include <cc/String>

namespace cc {

#define CC_XSTR(s) CC_STR(s)
#define CC_STR(s) #s

#define CC_BUNDLE_LOOKUP(relPath) Bundle::lookup(relPath, cc::StringList::create() << cc::Bundle::prefix(CC_XSTR(CCBUILD_BUNDLE_PREFIX)) << cc::Bundle::exePrefix() << String(__FILE__)->reducePath())
#define CC_BUNDLE_VERSION CC_XSTR(CCBUILD_BUNDLE_VERSION)

template<class>
class Singleton;

class Bundle: public Object
{
public:
    static String lookup(String relPath, const StringList *dirs);
    static String prefix(const char *defaultPrefix);
    static String exePrefix();

private:
    friend class Singleton<Bundle>;
    Bundle();
    String overridePrefix_;
    String exePrefix_;
};

} // namespace cc
