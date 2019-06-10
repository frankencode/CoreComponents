/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Format>
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

    virtual String machine() const override { return machine_; }
    virtual String systemRoot() const override { return systemRoot_; }
    virtual String compiler(const String &source = "") const override;
    virtual String compiler(const BuildPlan *plan) const override;
    virtual String machineCommand() const override;

    virtual String defaultOptimization(const BuildPlan *plan) const override;

    virtual String analyseCommand(const BuildPlan *plan, String source) const override;
    virtual Ref<Job> createAnalyseJob(const BuildPlan *plan, String source) const override;
    virtual Ref<Module> finishAnalyseJob(const BuildPlan *plan, const Job *job) const override;

    virtual Ref<Job> createCompileJob(const BuildPlan *plan, const Module *module) const override;
    virtual Ref<Job> createCompileLinkJob(const BuildPlan *plan, const Module *module) const override;

    virtual String targetName(const BuildPlan *plan) const override; // FIXME: does not need to be public
    virtual String linkName(const BuildPlan *plan) const override;
    virtual String linkCommand(const BuildPlan *plan) const override;
    virtual Ref<Job> createLinkJob(const BuildPlan *plan) const override;
    virtual bool link(const BuildPlan *plan) const override;

    virtual String configureCompileCommand(const BuildPlan *plan, String sourcePath, String binPath) const override;

    virtual String installDirPath(const BuildPlan *plan) const override;
    virtual String includePrefix(const BuildPlan *plan) const override;
    virtual String libIncludePrefix(const BuildPlan *plan) const override;
    virtual String bundlePrefix(const BuildPlan *plan) const override;

    virtual bool createSymlinks(const BuildPlan *plan) const override;

    virtual void createLibrarySymlinks(const BuildPlan *plan, String libName) const override;
    virtual void cleanLibrarySymlinks(const BuildPlan *plan, String libName) const override;

    virtual void createPluginSymlinks(const BuildPlan *plan, String targetLibName, String pluginLibName) const override;
    virtual void cleanPluginSymlinks(const BuildPlan *plan, String targetLibName, String pluginLibName) const override;
    virtual void cleanPluginSymlinks(const BuildPlan *plan, String targetLibName) const override;

    virtual void createAliasSymlinks(const BuildPlan *plan, String appName) const override;
    virtual void cleanAliasSymlinks(const BuildPlan *plan, String appName) const override;

    virtual String pkgConfigName(const BuildPlan *plan) const override;
    virtual String pkgConfigInstallDirPath(const BuildPlan *plan) const override;
    virtual String pkgConfig(const BuildPlan *plan) const override;
    virtual void generatePkgConfig(const BuildPlan *plan) const override;

    virtual bool refreshLinkerCache(const BuildPlan *plan) const override;

protected:
    GnuToolChain(const BuildPlan *plan);
    virtual ~GnuToolChain();

    static String queryMachine(const String &compiler);
    static String machineCommand(const String &compiler);

    static String querySystemRoot(String compiler);

    void appendCompileOptions(Format args, const BuildPlan *plan) const;
    void appendLinkOptions(Format args, const BuildPlan *plan) const;

private:
    String ccPath_;
    String cxxPath_;
    String machine_;
    String systemRoot_;
    Pattern dependencySplitPattern_;
    String rpathOverride_;
    bool isMultiArch_;
};

} // namespace ccbuild
