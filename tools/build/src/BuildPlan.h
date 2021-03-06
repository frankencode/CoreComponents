/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/syntax/PrefixTree>
#include <cc/Map>
#include <cc/Set>
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
using namespace cc::syntax;

class BuildPlan;
using BuildPlanList = List< Ref<BuildPlan> >;

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

    String projectPath() const { return projectPath_; }
    String systemSourcePath() const { return toolChain_->systemRoot()->extendPath("/usr/src"); }
    bool isSystemSource() const { return projectPath_->startsWith(systemSourcePath()); }
    static String recipePath(const String &projectPath) { return projectPath + "/Recipe"; }
    String recipePath() const { return recipePath_; }
    String userPkgConfigPath() const { return projectPath_->extendPath(toolChain_->pkgConfigName(this)); }
    String scope() const { return scope_; }
    String modulePath() const { return modulePath_; }
    String prestagePath() const { return prestagePath_; }
    String configPath() const { return configPath_; }
    MetaObject *recipe() const { return recipe_; }

    int options() const { return options_; }
    int concurrency() const { return concurrency_; }
    int testRunConcurrency() const { return testRunConcurrency_; }
    bool goForBuild() const;

    String name() const { return name_; }
    String description() const;
    StringList alias() const { return alias_; }
    Version version() const { return version_; }

    StringList sources() const { return sources_; }
    StringList bundle() const { return bundle_; }
    ModuleList &modules() { return modules_; }
    ModuleList modules() const { return modules_; }
    bool containsCPlusPlus() const { return containsCPlusPlus_; }

    PredicateList predicates() const { return predicates_; }
    BuildPlanList &prerequisites() { return prerequisites_; }
    BuildPlanList prerequisites() const { return prerequisites_; }
    SystemPrerequisitesByName systemPrerequisitesByName() const { return systemPrerequisitesByName_; }

    BuildPlan *extensionTarget() const { return extensionTarget_; }

    int run();

    String sourcePrefix() const { return sourcePrefix_; }
    String installRoot() const { return installRoot_; }
    String installPrefix() const { return installPrefix_; }
    String testArgs() const { return testArgs_; }

    String sourcePath(const String &source) const;
    String prestagePath(const String &precursor) const;
    String modulePath(const String &object) const;
    String installPath(const String &relativeInstallPath) const;

    String pluginPath(const String &targetLibName) const;
    String pluginReversePath() const;

    String previousLinkCommandPath() const;

    StringList globSources(const StringList &patternList) const;

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
    Ref<BuildPlan> create(const String &projectPath);

    BuildPlan();
    BuildPlan(int argc, char **argv);
    BuildPlan(const String &projectPath, BuildPlan *parentPlan);

    void readRecipe(BuildPlan *parentPlan = nullptr);
    void readPredicate(const MetaObject *object);
    void checkDuplicateTargetNames();
    void gatherAutoConfigureSystemPrerequisites(Set<String> &names);

    String findPrerequisite(const String &prerequisitePath) const;
    void readPrerequisites();
    void findVersion();

    void globSources();
    void initModules();

    static StringList queryableVariableNames();
    void queryVariables(const StringList &names) const;

    Ref<ToolChain> toolChain_;

    String projectPath_;
    String recipePath_;
    String scope_;
    String modulePath_;
    String configPath_;
    String prestagePath_;
    Ref<MetaObject> recipe_;

    int options_;
    int concurrency_;
    int testRunConcurrency_;
    bool disabled_;

    String name_;
    StringList alias_;
    Version version_;

    StringList sources_;
    StringList bundle_;
    ModuleList modules_;
    bool modulesInitialized_ { false };
    bool containsCPlusPlus_;

    PredicateList predicates_;
    SystemPrerequisitesByName systemPrerequisitesByName_;
    BuildPlanList prerequisites_;
    bool prerequisitesRead_ { false };
    Ref<BuildPlan> extensionTarget_;

    Ref<BuildParameters> usage_;

    String sourcePrefix_;
    String installRoot_;
    String installPrefix_;
    String testArgs_;

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
