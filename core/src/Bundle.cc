/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/File>
#include <cc/Process>
#include <cc/Singleton>
#include <cc/Bundle>

namespace cc {

String Bundle::lookup(String relPath, const StringList *dirs)
{
    return File::locate(relPath, dirs, FileAccess::Exists);
}

String Bundle::prefix(const char *defaultPrefix)
{
    if (Singleton<Bundle>::instance()->overridePrefix_ != "")
        return Singleton<Bundle>::instance()->overridePrefix_;
    return defaultPrefix;
}

String Bundle::exePrefix()
{
    return Singleton<Bundle>::instance()->exePrefix_;
}

Bundle::Bundle():
    overridePrefix_(Process::getEnv("CC_BUNDLE_PREFIX_OVERRIDE")),
    exePrefix_(Process::exePath()->reducePath())
{}

} // namespace cc
