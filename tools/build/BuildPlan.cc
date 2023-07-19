/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/BuildPlan>
#include <cc/build/RecipeProtocol>
#include <cc/build/PreparationStage>
#include <cc/build/ConfigureStage>
#include <cc/build/GlobbingStage>
#include <cc/build/CompileLinkStage>
#include <cc/build/TestRunStage>
#include <cc/build/InstallStage>
#include <cc/build/UninstallStage>
#include <cc/build/SystemPrerequisite>
#include <cc/build/Predicate>
#include <cc/build/ObjectFile>
#include <cc/build/GnuToolChain>
#include <cc/build/BuildShell>
#include <cc/build/ConfigureShell>
#include <cc/build/BuildMap>
#include <cc/build/Job>
#include <cc/MetaObject>
#include <cc/Version>
#include <cc/Map>
#include <cc/Set>
#include <cc/Arguments>
#include <cc/Process>
#include <cc/File>
#include <cc/Dir>
#include <cc/DirWalker>
#include <cc/Glob>
#include <cc/Crc32Sink>
#include <cc/JsonWriter>
#include <cc/ResourceGuard>
#include <cc/Format>
#include <cc/System>
#include <cc/yason>
#include <cc/stdio>

namespace cc::build {

struct BuildPlan::State:
    public BuildParameters::State,
    public PreparationStage,
    public ConfigureStage,
    public GlobbingStage,
    public CompileLinkStage,
    public TestRunStage,
    public InstallStage,
    public UninstallStage,
    public BuildShell
{
    State(int argc, char *argv[]):
        argv0_{argv[0]},
        projectPath_{"."}
    {
        Arguments arguments{argc, argv};
        List<String> items = arguments.items();

        if (items.count() > 0) {
            if (items.count() > 1) {
                throw UsageError{"Handling multiple source directories at once is not supported"};
            }
            projectPath_ = items.at(0);
        }

        projectPath_ = projectPath_.absolutePathRelativeTo(Process::cwd()).canonicalPath();
        recipePath_ = recipePath(projectPath_);

        recipe_ = yasonParse(File{recipePath_}.map(), RecipeProtocol{}).to<MetaObject>();
        arguments.validate(recipe_.members());
        arguments.override(&recipe_.members());

        readRecipe();
        readMakeFlags();

        Process::setEnv("PREFIX", installPrefix_);

        toolChain_ = GnuToolChain{compiler_, installPath("lib")};
        if (optimize_ == "") optimize_ = toolChain_.defaultOptimization(options_);

        scope_ = projectPath_;
    }

    State(const String &projectPath, const BuildPlan& parentPlan):
        toolChain_{parentPlan->toolChain_},
        projectPath_{projectPath},
        recipePath_{recipePath(projectPath)},
        scope_{parentPlan->scope_},
        concurrency_{parentPlan->concurrency_},
        disabled_{false}
    {
        ResourceGuard guard{recipePath_};
        recipe_ = yasonParse(File{recipePath_}.map(), RecipeProtocol{}).to<MetaObject>();
        readRecipe(parentPlan);
    }

    static String recipePath(const String &projectPath)
    {
        return projectPath / "Recipe";
    }

    String systemSourcePath() const
    {
        return toolChain_.systemRoot() / "usr/src";
    }

    void readRecipe(const BuildPlan &parentPlan = BuildPlan{})
    {
        if (recipe_.className() == "Application")  options_ |= BuildOption::Application;
        else if (recipe_.className() == "Library") options_ |= BuildOption::Library;
        else if (recipe_.className() == "Plugin")  options_ |= BuildOption::Library | BuildOption::Plugin;
        else if (recipe_.className() == "Test")    options_ |= BuildOption::Application | BuildOption::Test;
        else if (recipe_.className() == "Tools")   options_ |= BuildOption::Tools;
        else if (recipe_.className() == "Tests")   options_ |= BuildOption::Tools | BuildOption::Test;
        else if (recipe_.className() == "Package") options_ |= BuildOption::Package;

        name_ = recipe_("name").to<String>();
        if (name_ == "") name_ = projectPath_.baseName();
        alias_ = recipe_("alias").to<List<String>>();
        version_ = recipe_("version").to<Version>();
        installRoot_ = recipe_("root").to<String>();
        installPrefix_ = recipe_("prefix").to<String>();
        if (installPrefix_ == "") {
            installPrefix_ = (installRoot_ == "/") ? "/usr/local" : "/usr";
        }
        testArgs_ = recipe_("test-args").to<String>();

        checkDuplicateTargetNames();

        if (recipe_("debug"))       options_ |= BuildOption::Debug;
        if (recipe_("release"))     options_ |= BuildOption::Release;
        if (recipe_("simulate"))    options_ |= BuildOption::Simulate;
        if (recipe_("blindfold"))   options_ |= BuildOption::Blindfold;
        if (recipe_("bootstrap"))   options_ |= BuildOption::Bootstrap | BuildOption::Simulate | BuildOption::Blindfold;
        if (recipe_("test"))        options_ |= BuildOption::BuildTests;
        if (recipe_("test-run"))    options_ |= BuildOption::BuildTests;
        if (recipe_("test-report")) options_ |= BuildOption::BuildTests;
        if (recipe_("verbose"))     options_ |= BuildOption::Verbose;
        if (recipe_("configure"))   options_ |= BuildOption::Configure;
        if (recipe_("insight"))     options_ |= BuildOption::Insight;
        if (recipe_("lump"))        options_ |= BuildOption::Lump;
        if (recipe_("strip"))       options_ |= BuildOption::Strip;

        concurrency_ = recipe_("jobs").to<long>();
        testRunConcurrency_ = recipe_("test-run-jobs").to<long>();

        if (parentPlan) {
            options_ &= ~BuildOption::GlobalOptions;
            options_ |= parentPlan->options_ & BuildOption::GlobalOptions;
            concurrency_ = parentPlan->concurrency_;
            if (parentPlan->testRunConcurrency_ > 0 && testRunConcurrency_ < 0)
                testRunConcurrency_ = parentPlan->testRunConcurrency_;
            installRoot_ = parentPlan->installRoot_;
            installPrefix_ = parentPlan->installPrefix_;
            testArgs_ = parentPlan->testArgs_;
        }
        else {
            ConfigureShell{}.setVerbose(options_ & BuildOption::Verbose);
        }

        if (concurrency_ > 0 && testRunConcurrency_ < 0) {
            testRunConcurrency_ = concurrency_;
        }

        if (concurrency_ <= 0) concurrency_ = System::concurrency();

        BuildParameters::State::read(recipe_, projectPath_, options_);

        String defaultIncludePath = projectPath_ / "include";
        if (Dir::exists(defaultIncludePath)) {
            if (!includePaths_.contains(defaultIncludePath))
                includePaths_.append(defaultIncludePath);
        }

        List<String> dependsList = recipe_("depends").to<List<String>>();
        if (dependsList.count() > 0) {
            for (const String &item: dependsList) {
                String name;
                Version versionMin, versionMax;
                if (item.contains(">=")) {
                    List<String> parts = item.split(">=");
                    name = parts.at(0).trimmed();
                    if (parts.count() > 1) versionMin = Version{parts.at(1).trimmed()};
                }
                else if (item.contains("<=")) {
                    List<String> parts = item.split("<=");
                    name = parts.at(0).trimmed();
                    if (parts.count() > 1) versionMax = Version{parts.at(1).trimmed()};
                }
                else {
                    name = item;
                }

                SystemPrerequisite prerequisite{name};
                prerequisite.setVersionMin(versionMin);
                prerequisite.setVersionMax(versionMax);
                systemPrerequisitesByName_.insert(name, prerequisite);
            }
        }

        if (recipe_.children().count() > 0) {
            for (const MetaObject &object: recipe_.children()) {
                if (object.className() == "Dependency") {
                    SystemPrerequisite p{object, projectPath_, options_};
                    if (systemPrerequisitesByName_.find(p.name())) {
                        throw UsageError{Format{"%%: Ambiguous system dependency '%%'"} << recipePath_ << p.name()};
                    }
                    systemPrerequisitesByName_.insert(p.name(), p);
                }
                else if (object.className() == "Predicate") {
                    readPredicate(object);
                }
                else if (object.className() == "PreBuild") {
                    CompileLinkStage::preCommands().append(object("execute").to<String>());
                }
                else if (object.className() == "PostBuild") {
                    CompileLinkStage::postCommands().append(object("execute").to<String>());
                }
                else if (object.className() == "PreInstall") {
                    InstallStage::preCommands().append(object("execute").to<String>());
                }
                else if (object.className() == "PostInstall") {
                    InstallStage::postCommands().append(object("execute").to<String>());
                }
                else if (object.className() == "PreUninstall") {
                    UninstallStage::preCommands().append(object("execute").to<String>());
                }
                else if (object.className() == "PostUninstall") {
                    UninstallStage::postCommands().append(object("execute").to<String>());
                }
            }
        }

        if (parentPlan) {
            optimize_ = parentPlan->optimize_;
            linkStatic_ = parentPlan->linkStatic_;
        }
    }

    /** Respect MAKEFLAGS environment variable commonly used by meta build systems
      * (e.g. to support distcc builds).
      */
    void readMakeFlags()
    {
        String flags = Process::env("MAKEFLAGS");
        if (flags == "") return;
        long k = 0;
        if (flags.find("--jobs=", &k)) {
            k += 7;
        }
        else {
            k = 0;
            if (flags.find("-j", &k))
                k += 2;
            else
                k = 0;
        }
        if (k > 0) {
            while (k < flags.count() && flags.at(k) == ' ') ++k;
            long l = k;
            while (l < flags.count() && flags.at(l) != ' ') ++l;
            bool ok = false;
            int jobs = flags.copy(k, l).toInt(&ok);
            if (ok && jobs > 0) {
                jobs -= (jobs > 1 && jobs == System::concurrency());
                concurrency_ = jobs;
            }
        }
    }

    void readPredicate(const MetaObject &object)
    {
        predicates_.append(Predicate{object});
        for (const MetaObject &child: object.children())
            readPredicate(child);
    }

    void checkDuplicateTargetNames()
    {
        if (name_ == "") return;

        String otherRecipePath;
        bool ok = true;

        if (options_ & BuildOption::Library)
            ok = BuildMap{}.registerLibrary(name_, recipePath_, &otherRecipePath);
        else if (options_ & BuildOption::Application)
            ok = BuildMap{}.registerApplication(name_, recipePath_, &otherRecipePath);

        if (!ok) {
            throw UsageError{
                Format{"Duplicate target name '%%' in\n  %%\n  and\n  %%"}
                << name_
                << otherRecipePath
                << recipePath_
            };
        }
    }

    String findPrerequisite(const String &prerequisitePath) const
    {
        if (prerequisitePath.isAbsolutePath()) {
            if (File::exists(recipePath(prerequisitePath))) return prerequisitePath;
            return String{};
        }

        {
            String candidatePath = systemSourcePath() / prerequisitePath;
            if (File::exists(recipePath(candidatePath))) return candidatePath;
        }

        for (String path = projectPath_; !path.isRootPath(); path = path.cdUp()) {
            String candidatePath = path / prerequisitePath;
            if (File::exists(recipePath(candidatePath))) {
                candidatePath = candidatePath.canonicalPath();
                if (candidatePath == projectPath_) continue;
                if (!(options_ & BuildOption::Package)) {
                    if (File::exists(candidatePath / "src" / "Recipe")) {
                        candidatePath = candidatePath / "src";
                    }
                }
                return candidatePath;
            }
        }

        return String{};
    }

    void readPrerequisites(BuildPlan &self)
    {
        if (prerequisitesRead_) return;
        prerequisitesRead_ = true;

        if ((options_ & BuildOption::Test) && !(options_ & BuildOption::BuildTests)) return;

        List<String> prerequisitePaths = recipe_("use").to<List<String>>();

        if (options_ & BuildOption::Package) {
            List<String> packageItems = recipe_("include").to<List<String>>();
            if (packageItems.count() > 0) prerequisitePaths = packageItems;
        }

        for (const String &prerequisitePath: prerequisitePaths) {
            String path = findPrerequisite(prerequisitePath);
            if (path == "") {
                throw UsageError{Format{} << recipePath_ << ": Failed to locate prerequisite '" << prerequisitePath << "'"};
            }
            BuildPlan plan = self.loadChild(path);
            plan.readPrerequisites();
            prerequisites_.append(plan);
        }

        if (options_ & BuildOption::Plugin) {
            String extensionTargetPath = recipe_("extend").to<String>();
            if (extensionTargetPath == "")
                throw UsageError{Format{} << recipePath_ << ": Please provide the path of a library to extend in Plugin.extend"};
            String path = findPrerequisite(extensionTargetPath);
            if (path == "")
                throw UsageError{Format{} << recipePath_ << ": Failed to locate library '" << extensionTargetPath << "'"};
            extensionTarget_ = self.loadChild(path);
            if (extensionTarget_.options() & BuildOption::Package) {
                extensionTarget_.readPrerequisites();
                for (const BuildPlan &prerequisite: extensionTarget_.prerequisites()) {
                    if (prerequisite.options() & BuildOption::Library) {
                        extensionTarget_ = prerequisite;
                        break;
                    }
                }
            }
            if (!(extensionTarget_.options() & BuildOption::Library))
                throw UsageError{Format{} << recipePath_ << ": '" << extensionTargetPath << "' (Plugin.extend) does not point to a library"};
            extensionTarget_.readPrerequisites();
            prerequisites_.appendList(extensionTarget_.prerequisites());
            prerequisites_.append(extensionTarget_);
        }

        findVersion();
    }

    void findVersion()
    {
        if (version_.isValid()) return;

        BuildPlan plan;
        String versionPath;

        String path = projectPath_.cdUp();

        for (; !path.isRootPath(); path = path.cdUp())
        {
            if (lookupCache(path, &plan)) {
                if (plan.version().isValid()) {
                    version_ = plan.version();
                    break;
                }
                continue;
            }

            try {
                File file{recipePath(path)};
                String text = file.readAll();
                Variant value = yasonParse(text);
                if (value.is<MetaObject>()) {
                    MetaObject recipe = value.to<MetaObject>();
                    Variant value;
                    if (recipe.lookup("version", &value)) {
                        version_ = Version{str(value)};
                        if (version_.isValid()) break;
                    }
                }
            }
            catch (...) {
            }
        }

        if (!version_.isValid()) {
            version_ = Version{0, 1, 0};
        }
    }

    void gatherAutoConfigureSystemPrerequisites(Set<String> &names)
    {
        if (configureListComplete_) return;
        configureListComplete_ = true;

        for (auto &item: systemPrerequisitesByName_) {
            SystemPrerequisite &p = item.value();
            if (p.autoConfigure() && p.origName() != "")
                names.insert(p.origName());
        }

        for (auto &plan: prerequisites_) {
            plan->gatherAutoConfigureSystemPrerequisites(names);
        }
    }

    String prestagePath(const String &precursor) const
    {
        return prestagePath_ / precursor;
    }

    String sourcePath(const String &source) const
    {
        if (projectPath_ == ".") return source;
        return projectPath_ / source;
    }

    void init()
    {
        if (objectFilesInitialized_) return;

        if ((options_ & BuildOption::Test) && !(options_ & BuildOption::BuildTests)) return;

        objectFilesInitialized_ = true;

        String suffix;
        {
            Format f;
            String absoluteProjectPath = projectPath_.absolutePathRelativeTo(Process::cwd());
            {
                Format h;
                String topLevel = sourcePrefix_.absolutePathRelativeTo(Process::cwd());
                for (
                    String path = absoluteProjectPath;
                    path != topLevel && path != "/" && path != toolChain_.systemRoot();
                    path = path.cdUp()
                ) {
                    h << path.fileName();
                }
                h << topLevel.fileName();
                f << hex(crc32(h.reversed().join('/')), 8);
            }
            if (options_ & BuildOption::Bootstrap)
                f << "$MACHINE";
            else
                f << toolChain_.machine();
            f << absoluteProjectPath.cdUp().fileName() + "_" + absoluteProjectPath.fileName();
            suffix = f.join<String>('-');
        }
        objectsDirPath_ = ".objects-" + suffix;
        configPath_ = ".config-" + suffix;
        prestagePath_ = ".prestage-" + suffix;

        for (BuildPlan &prerequisite: prerequisites_) {
            prerequisite->init();
        }
    }

    String installPath(const String &relativeInstallPath) const
    {
        return installRoot_ / installPrefix_ / relativeInstallPath;
    }

    String installedPath(const String &relativeInstallPath) const
    {
        return installPrefix_ / relativeInstallPath;
    }

    void setupBuildDir() const
    {
        String options;
        {
            Format f;
            if (options_ & BuildOption::Debug) f << "-debug";
            if (options_ & BuildOption::Release) f << "-release";
            if (options_ & BuildOption::BuildTests) f << "-test";
            if (options_ & BuildOption::Verbose) f << "-verbose";
            if (options_ & BuildOption::Insight) f << "-insight";
            if (options_ & BuildOption::Strip) f << "-strip";
            if (recipe_("optimize") != "") f << ("-optimize=" + optimize_);
            if (recipe_("prefix") != "") f << ("-prefix=" + installPrefix_);
            if (recipe_("root") != "/") f << ("-root=" + installRoot_);
            f << projectPath_;
            options = f.join<String>(' ');
        }

        Dir::establish(".setup");

        String toolPath = Process::execPath();
        if (toolPath == "") toolPath = "ccbuild";

        Format{
            File{".setup/configure", FileOpen::Overwrite, FileMode::Default|FileMode::AnyExec}
        }
            << "#!/bin/sh -ex" << nl
            << "clear && " << toolPath << " -configure " << options << nl;

        Format{
            File{".setup/build", FileOpen::Overwrite, FileMode::Default|FileMode::AnyExec}
        }
            << "#!/bin/sh -ex" << nl
            << "clear && " << toolPath << " " << options << nl;

        Format{
            File{".setup/test_run", FileOpen::Overwrite, FileMode::Default|FileMode::AnyExec}
        }
            << "#!/bin/sh -ex" << nl
            << "clear && " << toolPath << " -test-run " << options << nl;


        Format{
            File{".setup/install", FileOpen::Overwrite, FileMode::Default|FileMode::AnyExec}
        }
            << "#!/bin/sh -ex" << nl
            << "clear && " << toolPath << " -install " << options << nl;

        Format{
            File{".setup/uninstall", FileOpen::Overwrite, FileMode::Default|FileMode::AnyExec}
        }
            << "#!/bin/sh -ex" << nl
            << "clear && " << toolPath << " -uninstall " << options << nl;

        try { File::unlink("build"); } catch (...) {};
        File::symlink(".setup/build", "build");

        File{"Makefile", FileOpen::Overwrite, FileMode::Default|FileMode::AnyExec}
        .write(
            ".PHONY: build configure install uninstall\n"
            "\n"
            "build:\n"
            "\t./.setup/build\n"
            "\n"
            "test:\n"
            "\t./.setup/test_run\n"
            "\n"
            "configure:\n"
            "\t./.setup/configure\n"
            "\n"
            "install:\n"
            "\t./.setup/install\n"
            "\n"
            "uninstall:\n"
            "\t./.setup/uninstall\n"
            "\n"
        );

        try { File::unlink("src"); } catch (...) {}
        File::symlink(BuildMap{}.commonPrefix(), "src");
    }

    BuildPlan plan() const override { return Object::alias<BuildPlan>(this); }

    const BuildShell &shell() const { return *this; }

    const char *argv0_ { nullptr };
    ToolChain toolChain_;

    String projectPath_;
    String recipePath_;
    String scope_;
    String objectsDirPath_;
    String configPath_;
    String prestagePath_;
    MetaObject recipe_;

    BuildOption options_  { BuildOption::None };
    int concurrency_ { -1 };
    int testRunConcurrency_ { -1 };
    bool disabled_ { false };

    String name_;
    List<String> alias_;
    Version version_;

    List<String> sources_;
    List<String> bundle_;
    List<ObjectFile> objectFiles_;
    bool objectFilesInitialized_ { false };
    bool containsCPlusPlus_ { false };

    List<Predicate> predicates_;
    MultiMap<String, SystemPrerequisite> systemPrerequisitesByName_;
    List<BuildPlan> prerequisites_;
    bool prerequisitesRead_ { false };
    BuildPlan extensionTarget_;

    BuildParameters usage_;

    String sourcePrefix_;
    String installRoot_;
    String installPrefix_;
    String testArgs_;

    Job libraryLinkJob_;

    bool configureListComplete_ { false };
    bool globbingResourcesComplete_ { false };
};

BuildPlan::BuildPlan(int argc, char *argv[]):
    BuildPlan{new State{argc, argv}}
{}

BuildPlan::BuildPlan(const String &projectPath, const BuildPlan &parentPlan):
    BuildPlan{new State{projectPath, parentPlan}}
{
    BuildMap{}.insertPlan(projectPath, *this);
}

BuildPlan::BuildPlan(State *newState):
    BuildParameters{newState}
{
    BuildMap{}.insertPlan(projectPath(), *this);
}

bool BuildPlan::lookupCache(const String &projectPath, Out<BuildPlan> plan)
{
    return BuildMap{}.lookupPlan(projectPath.absolutePathRelativeTo(Process::cwd()), &plan);
}

BuildPlan BuildPlan::loadChild(const String &projectPath)
{
    BuildPlan plan;
    if (lookupCache(projectPath, &plan)) return plan;
    return BuildPlan{projectPath, *this};
}

int BuildPlan::run()
{
    readPrerequisites();
    globResources();

    if (recipe("setup")) {
        me().setupBuildDir();
        return 0;
    }

    me().init();

    if (recipe("configure-list")) {
        Set<String> names;
        me().gatherAutoConfigureSystemPrerequisites(names);
        for (auto name: names)
            fout() << name << nl;
        return 0;
    }

    Process::setEnv("SOURCE", projectPath());

    configureStage().run();

    String query = recipe("query").to<String>();
    if (query != "") {
        queryVariables(query.split(','));
        return 0;
    }
    else if (recipe("query-all").to<bool>()) {
        queryVariables(queryableVariableNames());
        return 0;
    }

    if (options() & BuildOption::Configure) {
        return configureStage().success() ? 0 : 1;
    }

    if (options() & BuildOption::Bootstrap) {
        fout(
            "#!/bin/sh -ex\n"
            "SOURCE=$1\n"
            "MACHINE=$(%%)\n"
        ) << toolChain().machineCommand();
    }

    if (configureStage().success()) {
        if (!preparationStage().run()) {
            return 1;
        }
    }

    if (recipe("prepare").to<bool>()) return 0;

    String defaultIncludePath = projectPath() / "include";
    if (Dir::exists(defaultIncludePath)) {
        if (!me().includePaths_.contains(defaultIncludePath))
            me().includePaths_.append(defaultIncludePath);
    }

    if (!globbingStage().run()) return 1;

    if (recipe("glob").to<bool>()) return 0;

    if (recipe("pkg-config").to<bool>()) {
        if (options() & BuildOption::Package) {
            for (const BuildPlan &prerequisite: prerequisites()) {
                if (prerequisite.options() & BuildOption::Library) {
                    fout() << toolChain().pkgConfig(prerequisite);
                    break;
                }
            }
        }
        else if (options() & BuildOption::Library) {
            fout() << toolChain().pkgConfig(*this);
        }
        return 0;
    }

    if (recipe("uninstall").to<bool>()) {
        return !uninstallStage().run();
    }

    if (!compileLinkStage().run()) {
        return 1;
    }

    if (recipe("install").to<bool>()) {
        if (!installStage().run()) return 1;
        if (installStage().linkerCacheDirty()) {
            if (!toolChain().refreshLinkerCache(*this))
                return 1;
        }
        return 0;
    }

    bool testRun = recipe("test-run").to<bool>();
    bool testReport = recipe("test-report").to<bool>();
    if (testRun || testReport) {
        if (testRun) Process::setEnv("TEST_RUN", "1");
        if (testReport) Process::setEnv("TEST_REPORT", "1");
        if (!testRunStage().run()) {
            return testRunStage().status();
        }
    }

    return 0;
}

void BuildPlan::readPrerequisites()
{
    return me().readPrerequisites(*this);
}

void BuildPlan::globResources()
{
    if (me().globbingResourcesComplete_) return;

    me().globbingResourcesComplete_ = true;

    for (BuildPlan &prerequisite: prerequisites()) {
        prerequisite.globResources();
    }

    if (!recipe().contains("bundle")) return;
    List<String> patternList = recipe("bundle").to<List<String>>();
    if (patternList.count() == 0) return;
    List<String> resources;
    for (const String &pattern: patternList) {
        Glob glob{sourcePath(pattern)};
        for (String path; glob.read(&path);) {
            try {
                DirWalker walker{path};
                bool isDir = false;
                for (String path; walker.read(&path, &isDir);) {
                    if (!isDir) resources.append(path);
                }
            }
            catch (...) {
                resources.append(path);
            }
        }
    }
    bundle() = resources;
}

List<String> BuildPlan::queryableVariableNames()
{
    return List<String> {
        "name",
        "version",
        "source",
        "include-paths",
        "library-paths",
        "libraries",
        "custom-compile-flags",
        "custom-link-flags"
    };
}

void BuildPlan::queryVariables(const List<String> &names) const
{
    MetaObject variables;
    variables("name") = name();
    variables("version") = version();
    variables("source") = recipe("source");
    variables("include-paths") = includePaths();
    variables("library-paths") = libraryPaths();
    variables("libraries") = libraries();
    variables("custom-compile-flags") = customCompileFlags();
    variables("custom-link-flags") = customLinkFlags();
    JsonWriter{IoStream::output()}.writeObject(variables, names);
    IoStream::output().write("\n");
}

const ToolChain &BuildPlan::toolChain() const
{
    return me().toolChain_;
}

const BuildShell &BuildPlan::shell() const
{
    return me().shell();
}

const MetaObject &BuildPlan::recipe() const
{
    return me().recipe_;
}

Variant BuildPlan::recipe(const String &name) const
{
    return me().recipe_(name);
}

BuildOption BuildPlan::options() const
{
    return me().options_;
}

int BuildPlan::concurrency() const
{
    return me().concurrency_;
}

int BuildPlan::testRunConcurrency() const
{
    return me().testRunConcurrency_;
}

bool BuildPlan::goForBuild() const
{
    return (me().options_ & BuildOption::Package) || configureStage().success();
}

String BuildPlan::name() const
{
    return me().name_;
}

List<String> BuildPlan::alias() const
{
    return me().alias_;
}

Version BuildPlan::version() const
{
    return me().version_;
}

String BuildPlan::description() const
{
    return me().recipe_("description").to<String>();
}

bool BuildPlan::containsCPlusPlus() const
{
    return me().containsCPlusPlus_;
}

bool &BuildPlan::containsCPlusPlus()
{
    return me().containsCPlusPlus_;
}
const List<String> &BuildPlan::sources() const
{
    return me().sources_;
}

List<String> &BuildPlan::sources()
{
    return me().sources_;
}

const List<String> &BuildPlan::bundle() const
{
    return me().bundle_;
}

List<String> &BuildPlan::bundle()
{
    return me().bundle_;
}

const List<ObjectFile> &BuildPlan::objectFiles() const
{
    return me().objectFiles_;
}

List<ObjectFile> &BuildPlan::objectFiles()
{
    return me().objectFiles_;
}

String BuildPlan::projectPath() const
{
    return me().projectPath_;
}

String BuildPlan::systemSourcePath() const
{
    return me().systemSourcePath();
}

bool BuildPlan::isSystemSource() const
{
    return projectPath().startsWith(systemSourcePath());
}

String BuildPlan::sourcePath(const String &source) const
{
    return me().sourcePath(source);
}

String BuildPlan::recipePath() const
{
    return me().recipePath_;
}

String BuildPlan::userPkgConfigPath() const
{
    return projectPath() / toolChain().pkgConfigName(*this);
}

String BuildPlan::scope() const
{
    return me().scope_;
}

String BuildPlan::objectFilePath() const
{
    return me().objectsDirPath_;
}

String BuildPlan::objectFilePath(const String &objectName) const
{
    return me().objectsDirPath_ / objectName;
}

String BuildPlan::configPath() const
{
    return me().configPath_;
}

String BuildPlan::prestagePath() const
{
    return me().prestagePath_;
}

String BuildPlan::prestagePath(const String &precursor) const
{
    return me().prestagePath(precursor);
}

String BuildPlan::sourcePrefix() const
{
    return me().sourcePrefix_;
}

String &BuildPlan::sourcePrefix()
{
    return me().sourcePrefix_;
}

String BuildPlan::installRoot() const
{
    return me().installRoot_;
}

String BuildPlan::installPrefix() const
{
    return me().installPrefix_;
}

String BuildPlan::installPath(const String &relativeInstallPath) const
{
    return me().installPath(relativeInstallPath);
}

String BuildPlan::installedPath(const String &relativeInstallPath) const
{
    return me().installedPath(relativeInstallPath);
}

String BuildPlan::pluginPath(const String &targetLibName) const
{
    String path = targetLibName / "plugins";
    String group = me().recipe_("group");
    if (group != "") path = path / group;
    return path;
}

String BuildPlan::pluginReversePath() const
{
    String group = me().recipe_("group");
    return group != "" ? "../../.." : "../..";
}

String BuildPlan::previousCompileCommandPath() const
{
    return objectFilePath(".command-compile");
}

String BuildPlan::previousLinkCommandPath() const
{
    return objectFilePath(".command-link");
}

void BuildPlan::setLibraryLinkJob(const Job &linkJob)
{
    me().libraryLinkJob_ = linkJob;
}

void BuildPlan::registerLinkDerivative(Job &linkJob)
{
    for (BuildPlan &prerequisite: prerequisites()) {
        if (prerequisite.options() & BuildOption::Package) {
            for (BuildPlan &child: prerequisite.prerequisites()) {
                if (child.options() & BuildOption::Library) {
                    if (child.libraryLinkJob()) {
                        child.libraryLinkJob().registerDerivative(linkJob);
                    }
                }
            }
        }
        else if (prerequisite.options() & BuildOption::Library) {
            if (prerequisite.libraryLinkJob()) {
                prerequisite.libraryLinkJob().registerDerivative(linkJob);
            }
        }
    }
}

String BuildPlan::testArgs() const
{
    return me().testArgs_;
}

bool BuildPlan::testReport() const
{
    return me().recipe_("test-report").to<bool>();
}

const List<Predicate> &BuildPlan::predicates() const
{
    return me().predicates_;
}

MultiMap<String, SystemPrerequisite> BuildPlan::systemPrerequisitesByName() const
{
    return me().systemPrerequisitesByName_;
}

const List<BuildPlan> &BuildPlan::prerequisites() const
{
    return me().prerequisites_;
}

List<BuildPlan> &BuildPlan::prerequisites()
{
    return me().prerequisites_;
}

const BuildPlan &BuildPlan::extensionTarget() const
{
    return me().extensionTarget_;
}

PreparationStage &BuildPlan::preparationStage()
{
    return me();
}

ConfigureStage &BuildPlan::configureStage()
{
    return me();
}

const ConfigureStage &BuildPlan::configureStage() const
{
    return me();
}

GlobbingStage &BuildPlan::globbingStage()
{
    return me();
}

CompileLinkStage &BuildPlan::compileLinkStage()
{
    return me();
}

TestRunStage &BuildPlan::testRunStage()
{
    return me();
}

InstallStage &BuildPlan::installStage()
{
    return me();
}

UninstallStage &BuildPlan::uninstallStage()
{
    return me();
}

Job BuildPlan::libraryLinkJob() const
{
    return me().libraryLinkJob_;
}

BuildPlan::State *BuildPlan::operator->()
{
    return &me();
}

const BuildPlan::State *BuildPlan::operator->() const
{
    return &me();
}

BuildPlan::State &BuildPlan::me()
{
    return Object::me.as<State>();
}

const BuildPlan::State &BuildPlan::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::build
