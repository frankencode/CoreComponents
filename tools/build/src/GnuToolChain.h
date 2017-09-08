/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Format>
#include <cc/regexp/RegExp>
#include "ToolChain.h"

namespace ccbuild {

using namespace cc;
using namespace cc::regexp;

class BuildPlan;

class GnuToolChain: public ToolChain
{
public:
    static Ref<GnuToolChain> create(const BuildPlan *plan);

    virtual String machine() const override { return machine_; }
    virtual String systemRoot() const override { return systemRoot_; }
    virtual String compiler(String source = "") const override;
    virtual String compiler(BuildPlan *plan) const override;
    virtual String machineCommand() const override;

    virtual String defaultOptimization(BuildPlan *plan) const override;

    virtual String analyseCommand(BuildPlan *plan, String source) const override;
    virtual Ref<Job> createAnalyseJob(BuildPlan *plan, String source) override;
    virtual Ref<Module> finishAnalyseJob(BuildPlan *plan, Job *job) override;

    virtual Ref<Job> createCompileJob(BuildPlan *plan, Module *module) override;
    virtual Ref<Job> createLinkJob(BuildPlan *plan, Module *module) override;

    virtual String linkName(BuildPlan *plan) const override;
    virtual String linkCommand(BuildPlan *plan) const override;
    virtual bool link(BuildPlan *plan) override;

    virtual bool testInclude(BuildPlan *plan, StringList *headers) const override;

    virtual String installDirPath(BuildPlan *plan) const override;
    virtual String bundlePrefix(BuildPlan *plan) const override;

    virtual void createLibrarySymlinks(BuildPlan *plan, String libName) const override;
    virtual void cleanLibrarySymlinks(BuildPlan *plan, String libName) const override;

    virtual void createAliasSymlinks(BuildPlan *plan, String appName) const override;
    virtual void cleanAliasSymlinks(BuildPlan *plan, String appName) const override;

protected:
    GnuToolChain(const BuildPlan *plan);
    virtual ~GnuToolChain();

    void clangColorWorkaround(String compiler);

    static String queryMachine(String compiler);
    static String machineCommand(String compiler);

    static String querySystemRoot(String compiler);

    void appendCompileOptions(Format args, BuildPlan *plan) const;
    void appendLinkOptions(Format args, BuildPlan *plan) const;

private:
    String ccPath_;
    String cxxPath_;
    String machine_;
    String systemRoot_;
    RegExp dependencySplitPattern_;
    String rpathOverride_;
    bool isMultiArch_;
};

} // namespace ccbuild
