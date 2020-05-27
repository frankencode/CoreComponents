/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/format>
#include <cc/glob/Pattern>
#include "ToolChain.h"

namespace ccbuild {

using namespace cc;
using namespace cc::glob;

class BuildPlan;

class GnuToolChain: public ToolChain
{
public:
    static Ref<GnuToolChain> create(const BuildPlan *plan);

    virtual string machine() const override { return machine_; }
    virtual string systemRoot() const override { return systemRoot_; }
    virtual string compiler(const string &source = "") const override;
    virtual string compiler(const BuildPlan *plan) const override;
    virtual string machineCommand() const override;

    virtual string defaultOptimization(const BuildPlan *plan) const override;

    virtual string analyseCommand(const BuildPlan *plan, const string &source) const override;
    virtual Ref<Job> createAnalyseJob(const BuildPlan *plan, const string &source) const override;
    virtual Ref<Module> finishAnalyseJob(const BuildPlan *plan, const Job *job) const override;

    virtual Ref<Job> createCompileJob(const BuildPlan *plan, const Module *module) const override;
    virtual Ref<Job> createCompileLinkJob(const BuildPlan *plan, const Module *module) const override;
    virtual Ref<Job> createPreprocessJob(const BuildPlan *plan, const Module *module) const override;

    virtual string targetName(const BuildPlan *plan) const override; // FIXME: does not need to be public
    virtual string linkName(const BuildPlan *plan) const override;
    virtual string linkCommand(const BuildPlan *plan) const override;
    virtual Ref<Job> createLinkJob(const BuildPlan *plan) const override;
    virtual bool link(const BuildPlan *plan) const override;

    virtual string configureCompileCommand(const BuildPlan *plan, const string &sourcePath, const string &binPath) const override;

    virtual string installDirPath(const BuildPlan *plan) const override;
    virtual string includePrefix(const BuildPlan *plan) const override;
    virtual string libIncludePrefix(const BuildPlan *plan) const override;
    virtual string bundlePrefix(const BuildPlan *plan) const override;

    virtual bool createSymlinks(const BuildPlan *plan) const override;

    virtual void createLibrarySymlinks(const BuildPlan *plan, const string &libName) const override;
    virtual void cleanLibrarySymlinks(const BuildPlan *plan, const string &libName) const override;

    virtual void createPluginSymlinks(const BuildPlan *plan, const string &targetLibName, const string &pluginLibName) const override;
    virtual void cleanPluginSymlinks(const BuildPlan *plan, const string &targetLibName, const string &pluginLibName) const override;
    virtual void cleanPluginSymlinks(const BuildPlan *plan, const string &targetLibName) const override;

    virtual void createAliasSymlinks(const BuildPlan *plan, const string &appName) const override;
    virtual void cleanAliasSymlinks(const BuildPlan *plan, const string &appName) const override;

    virtual string pkgConfigName(const BuildPlan *plan) const override;
    virtual string pkgConfigInstallDirPath(const BuildPlan *plan) const override;
    virtual string pkgConfig(const BuildPlan *plan) const override;
    virtual void generatePkgConfig(const BuildPlan *plan) const override;

    virtual bool refreshLinkerCache(const BuildPlan *plan) const override;

protected:
    GnuToolChain(const BuildPlan *plan);
    virtual ~GnuToolChain();

    static string queryMachine(const string &compiler);
    static string machineCommand(const string &compiler);

    static string querySystemRoot(const string &compiler);

    void appendCompileOptions(format args, const BuildPlan *plan) const;
    void appendLinkOptions(format args, const BuildPlan *plan) const;

    static void appendRelocationMode(format args, const BuildPlan *plan);

private:
    string ccPath_;
    string cxxPath_;
    string machine_;
    string systemRoot_;
    Pattern dependencySplitPattern_;
    string rpathOverride_;
    bool isMultiArch_;
    string cFlags_;
    string cxxFlags_;
    string lFlags_;
};

} // namespace ccbuild
