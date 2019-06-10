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

    virtual String compiler(const String &source = "") const = 0;
    virtual String compiler(const BuildPlan *plan) const = 0;
    virtual String machineCommand() const = 0;
    virtual String defaultOptimization(const BuildPlan *plan) const = 0;

    virtual String analyseCommand(const BuildPlan *plan, String source) const = 0;
    virtual Ref<Job> createAnalyseJob(const BuildPlan *plan, String source) const = 0;
    virtual Ref<Module> finishAnalyseJob(const BuildPlan *plan, const Job *job) const = 0;

    virtual Ref<Job> createCompileJob(const BuildPlan *plan, const Module *module) const = 0;
    virtual Ref<Job> createCompileLinkJob(const BuildPlan *plan, const Module *module) const = 0;

    virtual String targetName(const BuildPlan *plan) const = 0;
    virtual String linkName(const BuildPlan *plan) const = 0;
    virtual String linkCommand(const BuildPlan *plan) const = 0;
    virtual Ref<Job> createLinkJob(const BuildPlan *plan) const = 0;
    virtual bool link(const BuildPlan *plan) const = 0;

    virtual String configureCompileCommand(const BuildPlan *plan, String sourcePath, String binPath) const = 0;

    virtual String installDirPath(const BuildPlan *plan) const = 0;
    virtual String includePrefix(const BuildPlan *plan) const = 0;
    virtual String libIncludePrefix(const BuildPlan *plan) const = 0;
    virtual String bundlePrefix(const BuildPlan *plan) const = 0;

    virtual bool createSymlinks(const BuildPlan *plan) const = 0;

    virtual void createLibrarySymlinks(const BuildPlan *plan, String libName) const = 0;
    virtual void cleanLibrarySymlinks(const BuildPlan *plan, String libName) const = 0;

    virtual void createPluginSymlinks(const BuildPlan *plan, String targetLibName, String pluginLibName) const = 0;
    virtual void cleanPluginSymlinks(const BuildPlan *plan, String targetLibName, String pluginLibName) const = 0;
    virtual void cleanPluginSymlinks(const BuildPlan *plan, String targetLibName) const = 0;

    virtual void createAliasSymlinks(const BuildPlan *plan, String appName) const = 0;
    virtual void cleanAliasSymlinks(const BuildPlan *plan, String appName) const = 0;

    virtual String pkgConfigName(const BuildPlan *plan) const = 0;
    virtual String pkgConfigInstallDirPath(const BuildPlan *plan) const = 0;
    virtual String pkgConfig(const BuildPlan *plan) const = 0;
    virtual void generatePkgConfig(const BuildPlan *plan) const = 0;

    virtual bool refreshLinkerCache(const BuildPlan *plan) const = 0;
};

} // namespace ccbuild
