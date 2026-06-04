/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/bundling>
#include <cc/Process>
#include <cc/File>

namespace cc {

Bundle &Bundle::instance()
{
    static Bundle instance_;
    return instance_;
}

String Bundle::lookup(const String &relPath, const List<String> &dirs)
{
    return File::locate(relPath, dirs, FileAccess::Exists);
}

String Bundle::prefix(const char *defaultPrefix)
{
    Bundle &bundle = instance();
    if (bundle.overridePrefix_ != "")
        return bundle.overridePrefix_;
    return defaultPrefix;
}

String Bundle::exePrefix()
{
    return Bundle::instance().exePrefix_;
}

Bundle::Bundle():
    overridePrefix_{Process::env("CC_BUNDLE_PREFIX_OVERRIDE")},
    exePrefix_{Process::execPath().cdUp()}
{}

} // namespace cc
