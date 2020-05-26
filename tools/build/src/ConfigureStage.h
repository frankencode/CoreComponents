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

    string configureShell(const string &shellCommand);
    void makeUseOf(BuildPlan *other);

    bool probeBuild(const string &name, const string &probe) const;
    bool runConfigure(const string &name, const string &configure, string *output) const;

    static Ref<StringList> getFlags(const MetaObject *object, const string &propertyName);
};

} // namespace ccbuild
