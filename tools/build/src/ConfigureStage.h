/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "BuildStage.h"

namespace ccbuild {

class ConfigureStage: public BuildStage
{
public:
    ConfigureStage(BuildPlan *plan): BuildStage{plan} {}
    bool run();

    String configureShell(const String &shellCommand);
    void makeUseOf(BuildPlan *other);

    bool probeBuild(const String &name, const String &probe) const;
    bool runConfigure(String name, String configure, String *output) const;

    static Ref<StringList> getFlags(const MetaObject *object, String propertyName);
};

} // namespace ccbuild
