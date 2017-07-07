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

class GnuToolChain: public ToolChain
{
public:
    static Ref<GnuToolChain> create(String compiler);

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

    virtual bool install(BuildPlan *plan) override;
    virtual bool install(BuildPlan *plan, Module *module) override;
    virtual bool uninstall(BuildPlan *plan) override;
    virtual bool uninstall(BuildPlan *plan, Module *module) override;

    virtual void clean(BuildPlan *plan) override;

protected:
    GnuToolChain(String compiler);
    virtual ~GnuToolChain();

    void clangColorWorkaround(String compiler);

    static String queryMachine(String compiler);
    static String machineCommand(String compiler);

    static String querySystemRoot(String compiler);

    static String bundlePrefix(BuildPlan *plan);

    static void appendCompileOptions(Format args, BuildPlan *plan);
    void appendLinkOptions(Format args, BuildPlan *plan) const;

    static void createLibrarySymlinks(BuildPlan *plan, String libName);
    static void cleanLibrarySymlinks(BuildPlan *plan, String libName);

    static void createAliasSymlinks(BuildPlan *plan, String appName);
    static void cleanAliasSymlinks(BuildPlan *plan, String appName);

private:
    String ccPath_;
    String cxxPath_;
    String machine_;
    String systemRoot_;
    RegExp dependencySplitPattern_;
    String rpathOverride_;
};

} // namespace ccbuild
