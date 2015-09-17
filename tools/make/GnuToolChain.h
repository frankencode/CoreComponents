/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_GNUTOOLCHAIN_H
#define FLUXMAKE_GNUTOOLCHAIN_H

#include <flux/Format>
#include <flux/syntax/RegExp>
#include "ToolChain.h"

namespace fluxmake {

using namespace flux;
using namespace flux::syntax;

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
    virtual bool link(BuildPlan *plan);

    virtual bool includeTest(BuildPlan *plan, String includePath, StringList *testIncludes) const;
    virtual bool linkTest(BuildPlan *plan, String libraryPath, StringList *testLibraries) const;

    virtual bool testHeaderPath(BuildPlan *plan, String headerPath) const;

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

    static String bundlePrefix(BuildPlan *plan);

    static void appendCompileOptions(Format args, BuildPlan *plan);
    static void appendLinkOptions(Format args, BuildPlan *plan);

    static void createLibrarySymlinks(BuildPlan *plan, String libName);
    static void cleanLibrarySymlinks(BuildPlan *plan, String libName);

    static void createAliasSymlinks(BuildPlan *plan, String appName);
    static void cleanAliasSymlinks(BuildPlan *plan, String appName);

private:
    RegExp dependencySplitPattern_;
};

} // namespace fluxmake

#endif // FLUXMAKE_GNUTOOLCHAIN_H
