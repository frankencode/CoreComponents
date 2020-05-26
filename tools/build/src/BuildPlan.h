/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/PrefixTree>
#include <cc/Map>
#include <cc/Version>
#include "Module.h"
#include "BuildParameters.h"
#include "BuildShell.h"
#include "ToolChain.h"
#include "Predicate.h"
#include "SystemPrerequisite.h"
#include "PreparationStage.h"
#include "ConfigureStage.h"
#include "AnalyseStage.h"
#include "PreprocessStage.h"
#include "CompileLinkStage.h"
#include "TestRunStage.h"
#include "InstallStage.h"
#include "UninstallStage.h"
#include "CleanStage.h"

namespace cc { class FileStatus; }
namespace cc { namespace meta { class MetaObject; }}

namespace ccbuild {

using namespace cc;
using namespace cc::meta;

class BuildPlan;
typedef List< Ref<BuildPlan> > BuildPlanList;

class BuildPlan: public BuildParameters
{
public:
    enum Option {
        Application   = 1 << 0,
        Library       = 1 << 1,
        Plugin        = 1 << 2,
        Test          = 1 << 3,
        Tools         = 1 << 4,
        Package       = 1 << 5,
        Debug         = 1 << 6,
        Release       = 1 << 7,
        Simulate      = 1 << 8,
        Blindfold     = 1 << 9,
        Bootstrap     = 1 << 10,
        BuildTests    = 1 << 11,
        Verbose       = 1 << 12,
        Configure     = 1 << 13,
        Clean         = 1 << 14,
        Unspecified   = -1,
        GlobalOptions = Debug|
                        Release|
                        Simulate|
                        Blindfold|
                        Bootstrap|
                        BuildTests|
                        Verbose|
                        Configure
    };

    static Ref<BuildPlan> create(int argc, char **argv);

    ToolChain *toolChain() const { return toolChain_; }

    string projectPath() const { return projectPath_; }
    string systemSourcePath() const { return toolChain_->systemRoot()->extendPath("/usr/src"); }
    bool isSystemSource() const { return projectPath_->startsWith(systemSourcePath()); }
    static string recipePath(const string &projectPath) { return projectPath + "/Recipe"; }
    string recipePath() const { return recipePath_; }
    string userPkgConfigPath() const { return projectPath_->extendPath(toolChain_->pkgConfigName(this)); }
    string scope() const { return scope_; }
    string modulePath() const { return modulePath_; }
    string configPath() const { return configPath_; }
    MetaObject *recipe() const { return recipe_; }

    int options() const { return options_; }
    int concurrency() const { return concurrency_; }
    int testRunConcurrency() const { return testRunConcurrency_; }
    bool goForBuild() const;

    string name() const { return name_; }
    string description() const;
    StringList *alias() const { return alias_; }
    Version version() const { return version_; }

    StringList *sources() const { return sources_; }
    StringList *bundle() const { return bundle_; }
    ModuleList *modules() const { return modules_; }
    void setModules(ModuleList *newList) { modules_ = newList; }
    bool containsCPlusPlus() const { return containsCPlusPlus_; }

    PredicateList *predicates() const { return predicates_; }
    BuildPlanList *prerequisites() const { return prerequisites_; }
    SystemPrerequisitesByName *systemPrerequisitesByName() const { return systemPrerequisitesByName_; }

    BuildPlan *extensionTarget() const { return extensionTarget_; }

    int run();

    string sourcePrefix() const { return sourcePrefix_; }
    string installRoot() const { return installRoot_; }
    string installPrefix() const { return installPrefix_; }
    string testArgs() const { return testArgs_; }

    string sourcePath(const string &source) const;
    string modulePath(const string &object) const;
    string installPath(const string &relativeInstallPath) const;

    string pluginPath(const string &targetLibName) const;
    string pluginReversePath() const;

    string previousLinkCommandPath() const;

    Ref<StringList> globSources(StringList *pattern) const;

    const BuildShell *shell() const { return shell_; }
    void setLibraryLinkJob(Job *linkJob) { libraryLinkJob_ = linkJob; }
    void registerLinkDerivative(Job *linkJob);

    ConfigureStage *configureStage() { return &configureStage_; }
    PreparationStage *preparationStage() { return &preparationStage_; }
    AnalyseStage *analyseStage() { return &analyseStage_; }
    PreprocessStage *preprocessStage() { return &preprocessStage_; }
    CompileLinkStage *compileLinkStage() { return &compileLinkStage_; }
    TestRunStage *testRunStage() { return &testRunStage_; }
    InstallStage *installStage() { return &installStage_; }
    UninstallStage *uninstallStage() { return &uninstallStage_; }
    CleanStage *cleanStage() { return &cleanStage_; }

private:
    Ref<BuildPlan> create(const string &projectPath);

    BuildPlan();
    BuildPlan(int argc, char **argv);
    BuildPlan(const string &projectPath, BuildPlan *parentPlan);

    void readRecipe(BuildPlan *parentPlan = nullptr);
    void readPredicate(const MetaObject *object);
    void checkDuplicateTargetNames();
    void gatherAutoConfigureSystemPrerequisites(Set<string> *names);

    string findPrerequisite(const string &prerequisitePath) const;
    void readPrerequisites();
    void findVersion();

    void globSources();
    void initModules();

    static Ref<StringList> queryableVariableNames();
    void queryVariables(const StringList *names) const;

    Ref<ToolChain> toolChain_;

    string projectPath_;
    string recipePath_;
    string scope_;
    string modulePath_;
    string configPath_;
    Ref<MetaObject> recipe_;

    int options_;
    int concurrency_;
    int testRunConcurrency_;
    bool disabled_;

    string name_;
    Ref<StringList> alias_;
    Version version_;

    Ref<StringList> sources_;
    Ref<StringList> bundle_;
    Ref<ModuleList> modules_;
    bool containsCPlusPlus_;

    Ref<PredicateList> predicates_;
    Ref<SystemPrerequisitesByName> systemPrerequisitesByName_;
    Ref<BuildPlanList> prerequisites_;
    Ref<BuildPlan> extensionTarget_;

    Ref<BuildParameters> usage_;

    string sourcePrefix_;
    string installRoot_;
    string installPrefix_;
    string testArgs_;

    Ref<const BuildShell> shell_;
    Ref<Job> libraryLinkJob_;

    bool configureListComplete_ { false };
    PreparationStage preparationStage_;
    ConfigureStage configureStage_;
    AnalyseStage analyseStage_;
    PreprocessStage preprocessStage_;
    CompileLinkStage compileLinkStage_;
    TestRunStage testRunStage_;
    InstallStage installStage_;
    UninstallStage uninstallStage_;
    CleanStage cleanStage_;
};

} // namespace ccbuild
