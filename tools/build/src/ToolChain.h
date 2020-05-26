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
    virtual string machine() const = 0;
    virtual string systemRoot() const = 0;

    virtual string compiler(const string &source = "") const = 0;
    virtual string compiler(const BuildPlan *plan) const = 0;
    virtual string machineCommand() const = 0;
    virtual string defaultOptimization(const BuildPlan *plan) const = 0;

    virtual string analyseCommand(const BuildPlan *plan, const string &source) const = 0;
    virtual Ref<Job> createAnalyseJob(const BuildPlan *plan, const string &source) const = 0;
    virtual Ref<Module> finishAnalyseJob(const BuildPlan *plan, const Job *job) const = 0;

    virtual Ref<Job> createCompileJob(const BuildPlan *plan, const Module *module) const = 0;
    virtual Ref<Job> createCompileLinkJob(const BuildPlan *plan, const Module *module) const = 0;
    virtual Ref<Job> createPreprocessJob(const BuildPlan *plan, const Module *module) const = 0;

    virtual string targetName(const BuildPlan *plan) const = 0;
    virtual string linkName(const BuildPlan *plan) const = 0;
    virtual string linkCommand(const BuildPlan *plan) const = 0;
    virtual Ref<Job> createLinkJob(const BuildPlan *plan) const = 0;
    virtual bool link(const BuildPlan *plan) const = 0;

    virtual string configureCompileCommand(const BuildPlan *plan, const string &sourcePath, const string &binPath) const = 0;

    virtual string installDirPath(const BuildPlan *plan) const = 0;
    virtual string includePrefix(const BuildPlan *plan) const = 0;
    virtual string libIncludePrefix(const BuildPlan *plan) const = 0;
    virtual string bundlePrefix(const BuildPlan *plan) const = 0;

    virtual bool createSymlinks(const BuildPlan *plan) const = 0;

    virtual void createLibrarySymlinks(const BuildPlan *plan, const string &libName) const = 0;
    virtual void cleanLibrarySymlinks(const BuildPlan *plan, const string &libName) const = 0;

    virtual void createPluginSymlinks(const BuildPlan *plan, const string &targetLibName, const string &pluginLibName) const = 0;
    virtual void cleanPluginSymlinks(const BuildPlan *plan, const string &targetLibName, const string &pluginLibName) const = 0;
    virtual void cleanPluginSymlinks(const BuildPlan *plan, const string &targetLibName) const = 0;

    virtual void createAliasSymlinks(const BuildPlan *plan, const string &appName) const = 0;
    virtual void cleanAliasSymlinks(const BuildPlan *plan, const string &appName) const = 0;

    virtual string pkgConfigName(const BuildPlan *plan) const = 0;
    virtual string pkgConfigInstallDirPath(const BuildPlan *plan) const = 0;
    virtual string pkgConfig(const BuildPlan *plan) const = 0;
    virtual void generatePkgConfig(const BuildPlan *plan) const = 0;

    virtual bool refreshLinkerCache(const BuildPlan *plan) const = 0;
};

} // namespace ccbuild
