/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "Job.h"
#include "Module.h"

namespace ccbuild {

using namespace cc;

class BuildPlan;
class DependencyCache;

class ToolChain: public Object
{
public:
    virtual String machine() const = 0;
    virtual String systemRoot() const = 0;

    virtual String compiler(String source = "") const = 0;
    virtual String compiler(BuildPlan *plan) const = 0;
    virtual String machineCommand() const = 0;
    virtual String defaultOptimization(BuildPlan *plan) const = 0;

    virtual String analyseCommand(BuildPlan *plan, String source) const = 0;
    virtual Ref<Job> createAnalyseJob(BuildPlan *plan, String source) = 0;
    virtual Ref<Module> finishAnalyseJob(BuildPlan *plan, Job *job) = 0;

    virtual Ref<Job> createCompileJob(BuildPlan *plan, Module *module) = 0;
    virtual Ref<Job> createLinkJob(BuildPlan *plan, Module *module) = 0;

    virtual String linkName(BuildPlan *plan) const = 0;
    virtual String linkCommand(BuildPlan *plan) const = 0;
    virtual bool link(BuildPlan *plan) = 0;

    virtual bool testInclude(BuildPlan *plan, StringList *headers) const = 0;

    virtual String installDirPath(BuildPlan *plan) const = 0;
    virtual String includePrefix(BuildPlan *plan) const = 0;
    virtual String libIncludePrefix(BuildPlan *plan) const = 0;
    virtual String bundlePrefix(BuildPlan *plan) const = 0;

    virtual void createLibrarySymlinks(BuildPlan *plan, String libName) const = 0;
    virtual void cleanLibrarySymlinks(BuildPlan *plan, String libName) const = 0;

    virtual void createAliasSymlinks(BuildPlan *plan, String appName) const = 0;
    virtual void cleanAliasSymlinks(BuildPlan *plan, String appName) const = 0;
};

} // namespace ccbuild
