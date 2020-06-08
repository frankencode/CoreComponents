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

    String defaultOptimization(const BuildPlan *plan) const override;

    String analyseCommand(const BuildPlan *plan, const String &source) const override;
    Ref<Job> createAnalyseJob(const BuildPlan *plan, const String &source) const override;
    Ref<Module> finishAnalyseJob(const BuildPlan *plan, const Job *job) const override;

    Ref<Job> createCompileJob(const BuildPlan *plan, const Module *module) const override;
    Ref<Job> createCompileLinkJob(const BuildPlan *plan, const Module *module) const override;
    Ref<Job> createPreprocessJob(const BuildPlan *plan, const Module *module) const override;

    String targetName(const BuildPlan *plan) const override; // FIXME: does not need to be public
    String linkName(const BuildPlan *plan) const override;
    String linkCommand(const BuildPlan *plan) const override;
    Ref<Job> createLinkJob(const BuildPlan *plan) const override;
    bool link(const BuildPlan *plan) const override;

    String configureCompileCommand(const BuildPlan *plan, const String &sourcePath, const String &binPath) const override;

    String installDirPath(const BuildPlan *plan) const override;
    String includePrefix(const BuildPlan *plan) const override;
    String libIncludePrefix(const BuildPlan *plan) const override;
    String bundlePrefix(const BuildPlan *plan) const override;

    bool createSymlinks(const BuildPlan *plan) const override;

    void createLibrarySymlinks(const BuildPlan *plan, const String &libName) const override;
    void cleanLibrarySymlinks(const BuildPlan *plan, const String &libName) const override;

    void createPluginSymlinks(const BuildPlan *plan, const String &targetLibName, const String &pluginLibName) const override;
    void cleanPluginSymlinks(const BuildPlan *plan, const String &targetLibName, const String &pluginLibName) const override;
    void cleanPluginSymlinks(const BuildPlan *plan, const String &targetLibName) const override;

    void createAliasSymlinks(const BuildPlan *plan, const String &appName) const override;
    void cleanAliasSymlinks(const BuildPlan *plan, const String &appName) const override;

    String pkgConfigName(const BuildPlan *plan) const override;
    String pkgConfigInstallDirPath(const BuildPlan *plan) const override;
    String pkgConfig(const BuildPlan *plan) const override;
    void generatePkgConfig(const BuildPlan *plan) const override;

    bool refreshLinkerCache(const BuildPlan *plan) const override;

protected:
    GnuToolChain(const BuildPlan *plan);
    virtual ~GnuToolChain();

    static String queryMachine(const String &compiler);
    static String machineCommand(const String &compiler);

    static String querySystemRoot(const String &compiler);

    void appendCompileOptions(Format &args, const BuildPlan *plan) const;
    void appendLinkOptions(Format &args, const BuildPlan *plan) const;

    static void appendRelocationMode(Format &args, const BuildPlan *plan);

private:
    String ccPath_;
    String cxxPath_;
    String machine_;
    String systemRoot_;
    Pattern dependencySplitPattern_;
    String rpathOverride_;
    bool isMultiArch_;
    String cFlags_;
    String cxxFlags_;
    String lFlags_;
};

} // namespace ccbuild
