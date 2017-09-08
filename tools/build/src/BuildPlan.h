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
#include "ConfigureStage.h"
#include "PreparationStage.h"
#include "AnalyseStage.h"
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
        Tools         = 1 << 2,
        Test          = 1 << 3,
        Package       = 1 << 4,
        Debug         = 1 << 5,
        Release       = 1 << 6,
        Simulate      = 1 << 7,
        Blindfold     = 1 << 8,
        Bootstrap     = 1 << 9,
        BuildTests    = 1 << 10,
        Verbose       = 1 << 11,
        Configure     = 1 << 12,
        Clean         = 1 << 13,
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

    inline ToolChain *toolChain() const { return toolChain_; }

    inline String projectPath() const { return projectPath_; }
    inline String systemSourcePath() const { return toolChain_->systemRoot()->expandPath("/usr/src"); }
    inline bool isSystemSource() const { return projectPath_->beginsWith(systemSourcePath()); }
    inline static String recipePath(String projectPath) { return projectPath + "/Recipe"; }
    inline String recipePath() const { return recipePath_; }
    inline String scope() const { return scope_; }
    inline String modulePath() const { return modulePath_; }
    inline MetaObject *recipe() const { return recipe_; }

    inline int options() const { return options_; }
    inline int concurrency() const { return concurrency_; }
    inline int testRunConcurrency() const { return testRunConcurrency_; }
    bool goForBuild() const;

    inline String name() const { return name_; }
    inline StringList *alias() const { return alias_; }
    inline Version version() const { return version_; }

    inline StringList *sources() const { return sources_; }
    inline StringList *bundle() const { return bundle_; }
    inline ModuleList *modules() const { return modules_; }
    inline bool containsCPlusPlus() const { return containsCPlusPlus_; }

    inline PredicateList *predicates() const { return predicates_; }
    inline BuildPlanList *prerequisites() const { return prerequisites_; }
    inline SystemPrerequisitesByName *systemPrerequisitesByName() const { return systemPrerequisitesByName_; }

    inline BuildParameters *usage() const { return usage_; }

    int run();

    inline String sourcePrefix() const { return sourcePrefix_; }
    inline String installRoot() const { return installRoot_; }
    inline String installPrefix() const { return installPrefix_; }
    inline String testArgs() const { return testArgs_; }

    String sourcePath(String source) const;
    String modulePath(String object) const;
    String installPath(String relativeInstallPath) const;

    Ref<StringList> globSources(StringList *pattern) const;

    String generatePackageConfig() const;

    inline BuildShell *shell() { return &shell_; }

    inline ConfigureStage *configureStage() { return &configureStage_; }
    inline PreparationStage *preparationStage() { return &preparationStage_; }
    inline AnalyseStage *analyseStage() { return &analyseStage_; }
    inline CompileLinkStage *compileLinkStage() { return &compileLinkStage_; }
    inline TestRunStage *testRunStage() { return &testRunStage_; }
    inline InstallStage *installStage() { return &installStage_; }
    inline UninstallStage *uninstallStage() { return &uninstallStage_; }
    inline CleanStage *cleanStage() { return &cleanStage_; }

private:
    Ref<BuildPlan> create(String projectPath);

    BuildPlan(int argc, char **argv);
    BuildPlan(String projectPath, BuildPlan *parentPlan);

    void readRecipe(BuildPlan *parentPlan = 0);
    void readPredicate(const MetaObject *object);
    void checkDuplicateTargetNames();

    void use(BuildPlan *plan);
    String findPrerequisite(String prerequisitePath) const;
    void readPrerequisites();
    void findVersion();

    void globSources();
    void initModules();

    static Ref<StringList> queryableVariableNames();
    void queryVariables(const StringList *names) const;

    Ref<ToolChain> toolChain_;

    String projectPath_;
    String recipePath_;
    String scope_;
    String modulePath_;
    Ref<MetaObject> recipe_;

    int options_;
    int concurrency_;
    int testRunConcurrency_;
    bool disabled_;

    String name_;
    Ref<StringList> alias_;
    Version version_;

    Ref<StringList> sources_;
    Ref<StringList> bundle_;
    Ref<ModuleList> modules_;
    bool containsCPlusPlus_;

    Ref<PredicateList> predicates_;
    Ref<SystemPrerequisitesByName> systemPrerequisitesByName_;
    Ref<BuildPlanList> prerequisites_;

    Ref<BuildParameters> usage_;

    String sourcePrefix_;
    String installRoot_;
    String installPrefix_;
    String testArgs_;

    BuildShell shell_;

    ConfigureStage configureStage_;
    PreparationStage preparationStage_;
    AnalyseStage analyseStage_;
    CompileLinkStage compileLinkStage_;
    TestRunStage testRunStage_;
    InstallStage installStage_;
    UninstallStage uninstallStage_;
    CleanStage cleanStage_;
};

} // namespace ccbuild
