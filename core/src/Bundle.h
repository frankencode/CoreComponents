/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>

#define CC_XSTR(s) CC_STR(s)
#define CC_STR(s) #s

/// Lookup a path of a bundled ressource
#define CC_BUNDLE_LOOKUP(relPath) cc::Bundle::lookup(relPath, cc::StringList::create() << cc::Bundle::prefix(CC_XSTR(CCBUILD_BUNDLE_PREFIX)) << cc::Bundle::exePrefix() << cc::String(__FILE__)->reducePath())

/// Version of this application or library bundle (low-level C string)
#define CC_BUNDLE_VERSION CC_XSTR(CCBUILD_BUNDLE_VERSION)

namespace cc {

template<class>
class Singleton;

/** \class Bundle Bundle.h cc/Bundle
  * \ingroup sysinfo
  * \brief Lookup files in installation directories
  */
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
