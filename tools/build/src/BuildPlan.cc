/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/Process>
#include <cc/Crc32Sink>
#include <cc/File>
#include <cc/Dir>
#include <cc/DirWalker>
#include <cc/ResourceGuard>
#include <cc/Arguments>
#include <cc/regexp/Glob>
#include <cc/meta/yason>
#include <cc/meta/JsonWriter>
#include <cc/debug>
#include "BuildMap.h"
#include "DependencyCache.h"
#include "GnuToolChain.h"
#include "JobScheduler.h"
#include "RecipeProtocol.h"
#include "BuildPlan.h"

namespace ccbuild {

Ref<BuildPlan> BuildPlan::create(int argc, char **argv)
{
    return new BuildPlan(argc, argv);
}

Ref<BuildPlan> BuildPlan::create(String projectPath)
{
    Ref<BuildPlan> plan;
    if (buildMap()->lookupPlan(String(projectPath->absolutePathRelativeTo(Process::cwd())), &plan)) return plan;
    return new BuildPlan(projectPath, this);
}

#define CCBUILD_BUILDPLAN_COMPONENTS_INIT \
    shell_(this), \
    configureStage_(this), \
    preparationStage_(this), \
    analyseStage_(this), \
    compileLinkStage_(this), \
    testRunStage_(this), \
    installStage_(this), \
    uninstallStage_(this), \
    cleanStage_(this)

BuildPlan::BuildPlan(int argc, char **argv):
    projectPath_("."),
    concurrency_(-1),
    disabled_(false),
    CCBUILD_BUILDPLAN_COMPONENTS_INIT
{
    Ref<Arguments> arguments = Arguments::parse(argc, argv);
    const StringList *items = arguments->items();

    if (items->count() > 0) {
        if (items->count() > 1)
            throw UsageError("Handling multiple source directories at once is not supported");
        projectPath_ = items->at(0);
    }

    projectPath_ = projectPath_->absolutePathRelativeTo(Process::cwd())->canonicalPath();
    recipePath_ = recipePath(projectPath_);

    ResourceGuard context(recipePath_);
    recipe_ = yason::parse(File::open(recipePath_)->map(), recipeProtocol());
    arguments->validate(recipe_);
    arguments->override(recipe_);

    readRecipe();

    toolChain_ = GnuToolChain::create(compiler());
    if (optimize_ == "") optimize_ = toolChain_->defaultOptimization(this);

    buildMap()->insertPlan(projectPath_, this);
    scope_ = projectPath_;
}

BuildPlan::BuildPlan(String projectPath, BuildPlan *parentPlan):
    toolChain_(parentPlan->toolChain_),
    projectPath_(projectPath),
    recipePath_(recipePath(projectPath)),
    scope_(parentPlan->scope_),
    concurrency_(parentPlan->concurrency_),
    disabled_(false),
    CCBUILD_BUILDPLAN_COMPONENTS_INIT
{
    ResourceGuard context(recipePath_);
    recipe_ = yason::parse(File::open(recipePath_)->map(), recipeProtocol());
    readRecipe(parentPlan);
    buildMap()->insertPlan(projectPath_, this);
}

bool BuildPlan::goForBuild() const
{
    return (options_ & Package) || configureStage_.success();
}

void BuildPlan::readRecipe(BuildPlan *parentPlan)
{
    name_ = recipe_->value("name");
    alias_ = recipe_->value("alias");
    version_ = recipe_->value("version");
    installRoot_ = recipe_->value("root");
    installPrefix_ = recipe_->value("prefix");
    testArgs_ = recipe_->value("test-args");

    options_ = 0;

    if (recipe_->className() == "Application")  options_ |= Application;
    else if (recipe_->className() == "Test")    options_ |= Application | Test;
    else if (recipe_->className() == "Library") options_ |= Library;
    else if (recipe_->className() == "Tools")   options_ |= Tools;
    else if (recipe_->className() == "Tests")   options_ |= Tools | Test;
    else if (recipe_->className() == "Package") options_ |= Package;

    checkDuplicateTargetNames();

    if (recipe_->value("debug"))       options_ |= Debug;
    if (recipe_->value("release"))     options_ |= Release;
    if (recipe_->value("simulate"))    options_ |= Simulate;
    if (recipe_->value("blindfold"))   options_ |= Blindfold;
    if (recipe_->value("bootstrap"))   options_ |= Bootstrap | Simulate | Blindfold;
    if (recipe_->value("test"))        options_ |= BuildTests;
    if (recipe_->value("test-run"))    options_ |= BuildTests;
    if (recipe_->value("test-report")) options_ |= BuildTests;
    if (recipe_->value("clean"))       options_ |= BuildTests;
    if (recipe_->value("verbose"))     options_ |= Verbose;
    if (recipe_->value("configure"))   options_ |= Configure;
    else if (recipe_->value("clean"))  options_ |= Clean;

    concurrency_ = recipe_->value("jobs");
    testRunConcurrency_ = recipe_->value("test-run-jobs");

    if (parentPlan) {
        options_ &= ~GlobalOptions;
        options_ |= parentPlan->options() & GlobalOptions;
        concurrency_ = parentPlan->concurrency_;
        if (parentPlan->testRunConcurrency_ > 0 && testRunConcurrency_ < 0)
            testRunConcurrency_ = parentPlan->testRunConcurrency_;
        installRoot_ = parentPlan->installRoot_;
        installPrefix_ = parentPlan->installPrefix_;
        testArgs_ = parentPlan->testArgs_;
    }

    if (concurrency_ > 0 && testRunConcurrency_ < 0)
        testRunConcurrency_ = concurrency_;

    BuildParameters::read(recipe_, this);

    String defaultIncludePath = projectPath_->expandPath("include");
    if (Dir::exists(defaultIncludePath)) {
        if (!includePaths_->contains(defaultIncludePath))
            includePaths_->append(defaultIncludePath);
    }

    if (recipe_->hasChildren()) {
        for (const MetaObject *object: recipe_->children()) {
            if (object->className() == "Dependency") {
                Ref<SystemPrerequisite> p = SystemPrerequisite::read(object, this);
                Ref<SystemPrerequisiteList> l;
                if (!systemPrerequisitesByName_)
                    systemPrerequisitesByName_ = SystemPrerequisitesByName::create();
                if (!systemPrerequisitesByName_->lookup(p->name(), &l))
                    systemPrerequisitesByName_->insert(p->name(), l = SystemPrerequisiteList::create());
                else
                    throw UsageError(Format("%%: Ambiguous system dependency '%%'") << recipePath_ << p->name());
                l->append(p);
            }
            else if (object->className() == "Predicate") {
                if (!predicates_) predicates_ = PredicateList::create();
                readPredicate(object);
            }
            else if (object->className() == "Usage") {
                usage_ = BuildParameters::create();
                usage_->read(object, this);
                BuildParameters::readSpecific(usage_);
            }
        }
    }

    if (parentPlan) {
        optimize_ = parentPlan->optimize();
        linkStatic_ = parentPlan->linkStatic();
    }
}

void BuildPlan::readPredicate(const MetaObject *object)
{
    predicates_->append(Predicate::read(object));
    for (const MetaObject *child: object->children())
        readPredicate(child);
}

void BuildPlan::checkDuplicateTargetNames()
{
    if (name_ == "") return;

    String otherRecipePath;
    bool ok = false;
    if (options_ & Library)
        ok = buildMap()->registerLibrary(name_, recipePath_, &otherRecipePath);
    else if (options_ & Application)
        ok = buildMap()->registerApplication(name_, recipePath_, &otherRecipePath);
    if (!ok) {
        throw UsageError(
            Format("Duplicate target name '%%' in\n  %%\n  and\n  %%")
            << name_
            << otherRecipePath
            << recipePath_
        );
    }
}

int BuildPlan::run()
{
    readPrerequisites();

    if (options_ & Bootstrap) {
        fout(
            "#!/bin/sh -ex\n"
            "SOURCE=$1\n"
            "MACHINE=$(%%)\n"
        ) << toolChain_->machineCommand();
    }

    if (!(options_ & Configure)) {
        if (!preparationStage()->run()) return 1;
        if (recipe_->value("prepare")) return 0;
    }

    String defaultIncludePath = projectPath_->expandPath("include");
    if (Dir::exists(defaultIncludePath)) {
        if (!includePaths_->contains(defaultIncludePath))
            includePaths_->append(defaultIncludePath);
    }

    recoverIncludeScope();
    globSources();
    initModules();

    configureStage()->run();
    if (String(recipe_->value("query")) != "") {
        queryVariables(Variant::cast<String>(recipe_->value("query"))->split(','));
        return 0;
    }
    else if (recipe_->value("query-all")) {
        queryVariables(queryableVariableNames());
        return 0;
    }
    if (options_ & Configure)
        return configureStage()->success() ? 0 : 1;

    if (!analyseStage()->run()) return 1;

    if (options_ & Clean) return !cleanStage()->run();
    if (recipe_->value("uninstall")) return !uninstallStage()->run();

    if (!compileLinkStage()->run()) return 1;

    if (recipe_->value("test-run") || recipe_->value("test-report")) {
        if (!testRunStage()->run())
            return testRunStage()->status();
    }

    if (recipe_->value("install")) return !installStage()->run();

    return 0;
}

String BuildPlan::sourcePath(String source) const
{
    if (projectPath_ == ".") return source;
    return projectPath_->expandPath(source);
}

String BuildPlan::modulePath(String object) const
{
    return modulePath_->expandPath(object);
}

String BuildPlan::installPath(String relativeInstallPath) const
{
    return installRoot_->expandPath(installPrefix_)->expandPath(relativeInstallPath);
}

Ref<StringList> BuildPlan::globSources(StringList *pattern) const
{
    Ref<StringList> sources = StringList::create();
    for (int i = 0; i < pattern->count(); ++i) {
        Ref<Glob> glob = Glob::open(sourcePath(pattern->at(i)));
        for (String path; glob->read(&path);) {
            Ref<DirWalker> walker = DirWalker::tryOpen(path);
            if (walker) {
                bool isDir = false;
                for (String path; walker->read(&path, &isDir);) {
                    if (!isDir) sources->append(path);
                }
            }
            else
                sources->append(path);
        }
    }
    sources = sources->sort();
    return sources;
}

void BuildPlan::use(BuildPlan *plan)
{
    if (plan->options() & Library) {
        String path = plan->projectPath();
        String defaultIncludePath = path->expandPath("include");
        if (Dir::exists(defaultIncludePath)) {
            if (!includePaths_->contains(defaultIncludePath))
                includePaths_->append(defaultIncludePath);
        }
        else if (!includePaths_->contains(path)) {
            includePaths_->append(path);
        }
        libraries_->append(plan->name());

        if (plan->usage()) BuildParameters::readSpecific(plan->usage());
    }
    else if (plan->options() & Package) {
        for (int i = 0; i < plan->prerequisites()->count(); ++i)
            use(plan->prerequisites()->at(i));
    }
}

String BuildPlan::findPrerequisite(String prerequisitePath) const
{
    if (prerequisitePath->isAbsolutePath()) {
        if (File::exists(recipePath(prerequisitePath))) return prerequisitePath;
        return String();
    }
    {
        String candidatePath = systemSourcePath()->expandPath(prerequisitePath);
        if (File::exists(recipePath(candidatePath))) return candidatePath;
    }
    for (String path = projectPath_; path != "/"; path = path->reducePath()) {
        String candidatePath = path->expandPath(prerequisitePath);
        if (File::exists(recipePath(candidatePath))) {
            candidatePath = candidatePath->canonicalPath();
            if (candidatePath == projectPath_) continue;
            return candidatePath;
        }
    }
    return String();
}

void BuildPlan::readPrerequisites()
{
    if (prerequisites_) return;

    prerequisites_ = BuildPlanList::create();

    if ((options_ & Test) && !(options_ & BuildTests)) return;

    const StringList *prerequisitePaths = Variant::cast<const StringList *>(recipe_->value("use"));
    if (options_ & Package) {
        const StringList *packageItems = Variant::cast<const StringList *>(recipe_->value("add"));
        if (packageItems->count() > 0) prerequisitePaths = packageItems;
    }

    for (String prerequisitePath: prerequisitePaths) {
        String path = findPrerequisite(prerequisitePath);
        if (path == "")
            throw UsageError(Format() << recipePath() << ": Failed to locate prerequisite \"" << prerequisitePath << "\"");
        Ref<BuildPlan> plan = BuildPlan::create(path);
        plan->readPrerequisites();
        use(plan);
        prerequisites_->append(plan);
    }

    findVersion();
}

void BuildPlan::findVersion()
{
    if (!Version::isValid(version_)) {
        String path = findPrerequisite("..");
        if (path != "") {
            Ref<BuildPlan> plan = BuildPlan::create(path);
            if (!Version::isValid(plan->version_)) plan->findVersion();
            if (Version::isValid(plan->version_)) version_ = plan->version_;
        }
    }
    if (!Version::isValid(version_))
        version_ = Version(0, 1, 0);
}

void BuildPlan::recoverIncludeScope()
{
    includeScope_ = StringList::create();

    if (!((options_ & Application) || (options_ & Library))) return;

    String path = projectPath_->expandPath("include");
    while (true) {
        Ref<Dir> dir = Dir::tryOpen(path);
        if (!dir) break;
        Ref<StringList> entries = StringList::create();
        for (String entry; dir->read(&entry);) {
            if (entry == "." || entry == "..") continue;
            if (Dir::tryOpen(path->expandPath(entry)))
                entries << entry;
        }
        if (entries->count() != 1) break;
        includeScope_ << entries->at(0);
        path = path->expandPath(entries->at(0));
    }
}

void BuildPlan::globSources()
{
    if (sources_) return;

    if ((options_ & Test) && !(options_ & BuildTests)) return;

    if (recipe_->contains("source"))
        sources_ = globSources(Variant::cast<StringList *>(recipe_->value("source")));
    else
        sources_ = StringList::create();

    sourcePrefix_ = buildMap()->commonPrefix();
    if (sourcePrefix_ == "") sourcePrefix_ = projectPath_;
    else sourcePrefix_ = sourcePrefix_->canonicalPath();

    containsCPlusPlus_ = false;
    for (String source: sources_) {
        String suffix = source->fileSuffix();
        if (suffix == "cpp" || suffix == "cc" || suffix == "cxx" || suffix == "mm") {
            containsCPlusPlus_ = true;
            break;
        }
    }

    if (recipe_->contains("bundle"))
        bundle_ = globSources(Variant::cast<StringList *>(recipe_->value("bundle")));
    else
        bundle_ = StringList::create();

    for (BuildPlan *plan: prerequisites_)
        plan->globSources();
}

void BuildPlan::initModules()
{
    if (modules_) return;

    if ((options_ & Test) && !(options_ & BuildTests)) return;

    modules_ = ModuleList::create();

    Format f;
    f << ".modules";
    String absoulteProjectPath = projectPath_->absolutePathRelativeTo(Process::cwd());
    {
        Format h;
        String topLevel = sourcePrefix_->absolutePathRelativeTo(Process::cwd());
        for (
            String path = absoulteProjectPath;
            path != topLevel && path != "/" && path != toolChain_->systemRoot();
            path = path->reducePath()
        )
            h << path->fileName();
        h << topLevel->fileName();
        f << hex(crc32(h->reverse()->join("/")), 8);
    }
    if (options_ & Bootstrap)
        f << "$MACHINE";
    else
        f << toolChain_->machine();
    f << absoulteProjectPath->reducePath()->fileName() + "_" + absoulteProjectPath->fileName();
    modulePath_ = f->join("-");

    for (int i = 0; i < prerequisites_->count(); ++i)
        prerequisites_->at(i)->initModules();
}

Ref<StringList> BuildPlan::queryableVariableNames()
{
    return StringList::create()
        << "name"
        << "version"
        << "build-scope"
        << "include-scope"
        << "include-paths"
        << "library-paths"
        << "libraries"
        << "custom-compile-flags"
        << "custom-link-flags";
}

void BuildPlan::queryVariables(const StringList *names) const
{
    Ref<MetaObject> variables = MetaObject::create();
    variables->insert("name", name_);
    variables->insert("version", version_);
    variables->insert("build-scope", scope_);
    variables->insert("include-scope", includeScope_);
    variables->insert("include-paths", includePaths_);
    variables->insert("library-paths", libraryPaths_);
    variables->insert("libraries", libraries_);
    variables->insert("custom-compile-flags", customCompileFlags_);
    variables->insert("custom-link-flags", customLinkFlags_);

    JsonWriter::create(stdOut())->writeObject(variables, names);
    stdOut()->write("\n");
}

} // namespace ccbuild
