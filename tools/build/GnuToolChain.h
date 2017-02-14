/*
 * Copyright (C) 2007-2016 Frank Mertens.
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

    virtual String machineCommand() const;

    virtual String defaultOptimization(BuildPlan *plan) const;

    virtual String analyseCommand(BuildPlan *plan, String source) const;
    virtual Ref<Job> createAnalyseJob(BuildPlan *plan, String source);
    virtual Ref<Module> finishAnalyseJob(BuildPlan *plan, Job *job);

    virtual Ref<Job> createCompileJob(BuildPlan *plan, Module *module);
    virtual Ref<Job> createLinkJob(BuildPlan *plan, Module *module);

    virtual String linkName(BuildPlan *plan) const;
    virtual String linkCommand(BuildPlan *plan) const;
    virtual bool link(BuildPlan *plan);

    virtual bool testInclude(BuildPlan *plan, StringList *headers) const;

    virtual bool install(BuildPlan *plan);
    virtual bool install(BuildPlan *plan, Module *module);
    virtual bool uninstall(BuildPlan *plan);
    virtual bool uninstall(BuildPlan *plan, Module *module);

    virtual void clean(BuildPlan *plan);

protected:
    GnuToolChain(String compiler);
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
    RegExp dependencySplitPattern_;
    String rpathOverride_;
};

} // namespace ccbuild
